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

#ifndef FUCHSIA_FLATLAND_VIEW_H_
#define FUCHSIA_FLATLAND_VIEW_H_

#include <fidl/fuchsia.io/cpp/fidl.h>
#include <fidl/fuchsia.ui.app/cpp/fidl.h>
#include <fidl/fuchsia.ui.composition/cpp/fidl.h>
#include <fidl/fuchsia.ui.views/cpp/fidl.h>
#include <lib/fit/function.h>
#include <lib/async/dispatcher.h>
#include <lib/syslog/cpp/macros.h>

#include <cstdint>
#include <utility>

#include <memory>

// This class provides a convenient wrapper around the scenic fidl apis needed to get a Flatland
// View. Only SDK available constructs should be used so that this code may be leveraged by Vulkan
// applications built with the SDK.
class FlatlandView : public fidl::AsyncEventHandler<fuchsia_ui_composition::Flatland> {
  public:
    using ResizeCallback = fit::function<void(uint32_t width, uint32_t height)>;

    // Creates and initializes a `FlatlandView` instance.
    //
    // `dispatcher` must be non-null.
    static std::unique_ptr<FlatlandView> Create(fidl::UnownedClientEnd<fuchsia_io::Directory> service_directory,
                                                fuchsia_ui_views::ViewCreationToken view_creation_token,
                                                ResizeCallback resize_callback, async_dispatcher_t* dispatcher);

    // Production code must use the `Create()` factory method.
    //
    // `dispatcher` must be non-null.
    explicit FlatlandView(ResizeCallback resize_callback, async_dispatcher_t* dispatcher);

    fuchsia_ui_views::ViewCreationToken TakeChildViewCreationToken() {
        FX_DCHECK(child_view_creation_token_.value().is_valid());
        return std::move(child_view_creation_token_);
    }

    bool Init(fidl::UnownedClientEnd<fuchsia_io::Directory> service_directory,
              fuchsia_ui_views::ViewCreationToken view_creation_token);

  private:
    void OnNextFrameBegin(fidl::Event<fuchsia_ui_composition::Flatland::OnNextFrameBegin>& event) override;
    void OnFramePresented(fidl::Event<fuchsia_ui_composition::Flatland::OnFramePresented>& event) override;
    void OnError(fidl::Event<fuchsia_ui_composition::Flatland::OnError>& event) override;

    void OnGetLayout(fuchsia_ui_composition::LayoutInfo info);
    void OnFlatlandError(fuchsia_ui_composition::FlatlandError error);
    void Present();

    fidl::SharedClient<fuchsia_ui_composition::Flatland> flatland_;
    fidl::SharedClient<fuchsia_ui_composition::ParentViewportWatcher> parent_viewport_watcher_;

    fuchsia_ui_views::ViewportCreationToken viewport_creation_token_;
    fuchsia_ui_views::ViewCreationToken child_view_creation_token_;
    ResizeCallback resize_callback_;

    int64_t present_credits_ = 1;
    bool pending_present_ = false;

    async_dispatcher_t* const dispatcher_;

    friend class FlatlandViewTest;
};

class FlatlandViewProviderService : public fidl::Server<fuchsia_ui_app::ViewProvider> {
  public:
    using CreateView2Callback = fit::function<void(fuchsia_ui_app::CreateView2Args args)>;

    // `dispatcher` must be non-null.
    explicit FlatlandViewProviderService(CreateView2Callback create_view_callback, async_dispatcher_t* dispatcher);

    // fuchsia::ui::app::ViewProvider methods.
    void CreateViewWithViewRef(CreateViewWithViewRefRequest& request, CreateViewWithViewRefCompleter::Sync& completer) override;
    void CreateView2(CreateView2Request& request, CreateView2Completer::Sync& completer) override;

    void HandleViewProviderRequest(fidl::ServerEnd<fuchsia_ui_app::ViewProvider> server_end);

  private:
    CreateView2Callback create_view_callback_;
    fidl::ServerBindingGroup<fuchsia_ui_app::ViewProvider> bindings_;

    async_dispatcher_t* const dispatcher_;
};

#endif  // FUCHSIA_FLATLAND_VIEW_H_
