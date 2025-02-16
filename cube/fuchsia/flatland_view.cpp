// Copyright (c) 2025 The Fuchsia Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "flatland_view.h"

#include <fidl/fuchsia.ui.app/cpp/fidl.h>
#include <fidl/fuchsia.ui.composition/cpp/fidl.h>
#include <fidl/fuchsia.ui.views/cpp/fidl.h>
#include <lib/component/incoming/cpp/protocol.h>
#include <lib/component/outgoing/cpp/outgoing_directory.h>
#include <lib/syslog/cpp/macros.h>
#include <lib/syslog/global.h>
#include <lib/ui/scenic/cpp/view_creation_tokens.h>
#include <lib/ui/scenic/cpp/view_identity.h>

#include <cinttypes>

namespace {

const char* const kTag = "FlatlandView";

const fuchsia_ui_composition::TransformId kRootTransform = {1};
const fuchsia_ui_composition::ContentId kViewport = {1};

}  // namespace

// static
std::unique_ptr<FlatlandView> FlatlandView::Create(fidl::UnownedClientEnd<fuchsia_io::Directory> service_directory,
                                                   fuchsia_ui_views::ViewCreationToken view_creation_token,
                                                   ResizeCallback resize_callback, async_dispatcher_t* dispatcher) {
    ZX_DEBUG_ASSERT(dispatcher != nullptr);
    auto view = std::make_unique<FlatlandView>(std::move(resize_callback), dispatcher);
    if (!view) return nullptr;
    if (!view->Init(service_directory, std::move(view_creation_token))) return nullptr;
    return view;
}

FlatlandView::FlatlandView(ResizeCallback resize_callback, async_dispatcher_t* dispatcher)
    : resize_callback_(std::move(resize_callback)), dispatcher_(dispatcher) {
    ZX_DEBUG_ASSERT(dispatcher != nullptr);
}

bool FlatlandView::Init(fidl::UnownedClientEnd<fuchsia_io::Directory> service_directory,
                        fuchsia_ui_views::ViewCreationToken view_creation_token) {
    zx::result<fidl::ClientEnd<fuchsia_ui_composition::Flatland>> connect_result =
        component::ConnectAt<fuchsia_ui_composition::Flatland>(service_directory);
    if (connect_result.is_error()) {
        FX_LOGS(ERROR) << "Failed to connect to Flatland: " << connect_result.status_string();
        return false;
    }
    fidl::ClientEnd<fuchsia_ui_composition::Flatland> flatland_client = std::move(connect_result).value();
    flatland_.Bind(std::move(flatland_client), dispatcher_, /*event_handler=*/this);
    fit::result<fidl::OneWayError> set_debug_name_result = flatland_->SetDebugName({{.name = kTag}});
    if (set_debug_name_result.is_error()) {
        FX_LOGS(ERROR) << "Failed to set debug name: " << set_debug_name_result.error_value().FormatDescription();
    }

    fit::result<fidl::OneWayError> create_transform_result = flatland_->CreateTransform(kRootTransform);
    if (create_transform_result.is_error()) {
        FX_LOGS(ERROR) << "Failed to call CreateTransform: " << create_transform_result.error_value().FormatDescription();
        return false;
    }

    fit::result<fidl::OneWayError> set_root_transform_result = flatland_->SetRootTransform(kRootTransform);
    if (set_root_transform_result.is_error()) {
        FX_LOGS(ERROR) << "Failed to call SetRootTransform: " << set_root_transform_result.error_value().FormatDescription();
        return false;
    }

    auto [parent_viewport_watcher_client, parent_viewport_watcher_server] =
        fidl::Endpoints<fuchsia_ui_composition::ParentViewportWatcher>::Create();
    fit::result<fidl::OneWayError> create_view2_result = flatland_->CreateView2({{
        .token = std::move(view_creation_token),
        .view_identity = scenic::cpp::NewViewIdentityOnCreation(),
        .protocols = {},
        .parent_viewport_watcher = std::move(parent_viewport_watcher_server),
    }});
    if (create_view2_result.is_error()) {
        FX_LOGS(ERROR) << "Failed to call CreateView2: " << set_root_transform_result.error_value().FormatDescription();
        return false;
    }

    parent_viewport_watcher_.Bind(std::move(parent_viewport_watcher_client), dispatcher_);
    parent_viewport_watcher_->GetLayout().Then(
        [this](fidl::Result<fuchsia_ui_composition::ParentViewportWatcher::GetLayout>& result) {
            if (result.is_error()) {
                FX_LOGS(ERROR) << "GetLayout() failed: " << result.error_value().FormatDescription();
                return;
            }
            OnGetLayout(std::move(result.value().info()));
        });

    zx::channel::create(0, &viewport_creation_token_.value(), &child_view_creation_token_.value());

    return true;
}

void FlatlandView::OnGetLayout(fuchsia_ui_composition::LayoutInfo info) {
    ZX_DEBUG_ASSERT(info.logical_size().has_value());
    const fuchsia_math::SizeU& logical_size = info.logical_size().value();
    resize_callback_(logical_size.width(), logical_size.height());

    fuchsia_ui_composition::ViewportProperties properties = {{.logical_size = logical_size}};
    if (viewport_creation_token_.value().is_valid()) {
        // The first time that we receive layout information, create a viewport using the token that was stashed during Init().
        // External code will attach a view to this viewport via the token obtained from TakeChildViewCreationToken().
        auto [child_view_watcher_client, child_view_watcher_server] =
            fidl::Endpoints<fuchsia_ui_composition::ChildViewWatcher>::Create();
        fit::result<fidl::OneWayError> create_viewport_result = flatland_->CreateViewport({{
            .viewport_id = kViewport,
            .token = std::move(viewport_creation_token_),
            .properties = std::move(properties),
            .child_view_watcher = std::move(child_view_watcher_server),
        }});
        if (create_viewport_result.is_error()) {
            FX_LOGS(ERROR) << "Failed to call CreateViewport(): " << create_viewport_result.error_value().FormatDescription();
            return;
        }

        fit::result<fidl::OneWayError> set_content_result = flatland_->SetContent({{
            .transform_id = kRootTransform,
            .content_id = kViewport,
        }});
        if (set_content_result.is_error()) {
            FX_LOGS(ERROR) << "Failed to call SetContent(): " << set_content_result.error_value().FormatDescription();
            return;
        }
    } else {
        auto set_viewport_properties_result = flatland_->SetViewportProperties({{
            .viewport_id = kViewport,
            .properties = std::move(properties),
        }});
        if (set_viewport_properties_result.is_error()) {
            FX_LOGS(ERROR) << "Failed to call SetViewportProperties(): "
                           << set_viewport_properties_result.error_value().FormatDescription();
            return;
        }
    }

    Present();
    parent_viewport_watcher_->GetLayout().Then(
        [this](fidl::Result<fuchsia_ui_composition::ParentViewportWatcher::GetLayout>& result) {
            if (result.is_error()) {
                FX_LOGS(ERROR) << "GetLayout() failed: " << result.error_value().FormatDescription();
                return;
            }
            OnGetLayout(std::move(result.value().info()));
        });
}

void FlatlandView::OnError(fidl::Event<fuchsia_ui_composition::Flatland::OnError>& event) {
    FX_LOGF(ERROR, kTag, "OnFlatlandError: %" PRIu32, static_cast<uint32_t>(event.error()));
}

void FlatlandView::Present() {
    if (present_credits_ == 0) {
        pending_present_ = true;
        return;
    }
    --present_credits_;
    fuchsia_ui_composition::PresentArgs present_args = {{
        .requested_presentation_time = 0,
        .acquire_fences = {},
        .release_fences = {},
        .unsquashable = false,
    }};
    fit::result<fidl::OneWayError> present_result = flatland_->Present(std::move(present_args));
    if (present_result.is_error()) {
        FX_LOGS(ERROR) << "Failed to call Present(): " << present_result.error_value().FormatDescription();
    }
}

void FlatlandView::OnFramePresented(fidl::Event<fuchsia_ui_composition::Flatland::OnFramePresented>& event) {}

void FlatlandView::OnNextFrameBegin(fidl::Event<fuchsia_ui_composition::Flatland::OnNextFrameBegin>& event) {
    present_credits_ += event.values().additional_present_credits().value_or(0);
    if (present_credits_ > 0 && pending_present_) {
        Present();
        pending_present_ = false;
    }
}

FlatlandViewProviderService::FlatlandViewProviderService(CreateView2Callback create_view_callback, async_dispatcher_t* dispatcher)
    : create_view_callback_(std::move(create_view_callback)), dispatcher_(dispatcher) {
    ZX_DEBUG_ASSERT(dispatcher != nullptr);
}

void FlatlandViewProviderService::CreateViewWithViewRef(CreateViewWithViewRefRequest& request,
                                                        CreateViewWithViewRefCompleter::Sync& completer) {
    FX_NOTIMPLEMENTED() << "Only Flatland is supported. This is a Gfx ViewProvider method.";
}

void FlatlandViewProviderService::CreateView2(CreateView2Request& request, CreateView2Completer::Sync& completer) {
    create_view_callback_(std::move(request.args()));
}

void FlatlandViewProviderService::HandleViewProviderRequest(fidl::ServerEnd<fuchsia_ui_app::ViewProvider> server_end) {
    bindings_.AddBinding(dispatcher_, std::move(server_end), this, fidl::kIgnoreBindingClosure);
}
