///
/// @file View.h
///
/// @brief The header for the View class.
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2020 Ultralight, Inc. All rights reserved.
///
#pragma once
#include <JavaScriptCore/JavaScript.h>
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/KeyEvent.h>
#include <Ultralight/MouseEvent.h>
#include <Ultralight/ScrollEvent.h>
#include <Ultralight/RenderTarget.h>
#include <Ultralight/Bitmap.h>
#include <Ultralight/Listener.h>
#include <Ultralight/platform/Surface.h>

namespace ultralight {

///
/// @brief A View is similar to a tab in a browser-- you load web content into
///	       it and display it however you want. @see Renderer::CreateView
///
/// @note  The API is currently not thread-safe, all calls must be made on the
///        main thread.
///
class UExport View : public RefCounted {
public:
  ///
  /// Get the URL of the current page loaded into this View, if any.
  ///
  virtual String url() = 0;

  ///
  /// Get the title of the current page loaded into this View, if any.
  ///
  virtual String title() = 0;

  ///
  /// Get the width of the View, in pixels.
  ///
  virtual uint32_t width() const = 0;

  ///
  /// Get the height of the View, in pixels.
  ///
  virtual uint32_t height() const = 0;

  ///
  /// Check if the main frame of the page is currently loading.
  ///
  virtual bool is_loading() = 0;

  ///
  /// Get the RenderTarget for the View.
  ///
  /// @note  Only valid when the GPU renderer is enabled in Config.
  ///
  ///        You can use this with your GPUDriver implementation to bind
  ///        and display the corresponding texture in your application.
  ///
  virtual RenderTarget render_target() = 0;

  ///
  /// Get the Surface for the View (native pixel buffer container).
  ///
  /// @note  Only valid when the GPU renderer is disabled in Config.
  ///
  ///        (Will return a nullptr when the GPU renderer is enabled.)
  ///
  ///        The default Surface is BitmapSurface but you can provide your
  ///        own Surface implementation via Platform::set_surface_factory.
  ///
  virtual Surface* surface() = 0;

  ///
  /// Load a raw string of HTML, the View will navigate to it as a new page.
  ///
  /// @param  html  The raw HTML string to load.
  ///
  /// @param  url   An optional URL for this load (to make it appear as if we
  ///               we loaded this HTML from a certain URL). Can be used for
  ///               resolving relative URLs and cross-origin rules.
  ///
  /// @param  add_to_history  Whether or not this load should be added to the
  ///                         session's history (back/forward list).
  ///
  virtual void LoadHTML(const String& html,
    const String& url = "", 
    bool add_to_history = false) = 0;

  ///
  /// Load a URL, the View will navigate to it as a new page.
  ///
  /// @note  You can use File URLs (eg, file:///page.html) but you must define
  ///        your own FileSystem implementation if you are not using AppCore.
  ///        @see Platform::set_file_system
  ///
  virtual void LoadURL(const String& url) = 0;

  ///
  /// Resize View to a certain size.
  ///
  /// @param  width   The initial width, in pixels.
  /// 
  /// @param  height  The initial height, in pixels.
  ///
  ///
  virtual void Resize(uint32_t width, uint32_t height) = 0;

  ///
  /// Get the page's JSContext for use with the JavaScriptCore API
  ///
  /// @note  We expose the entire JavaScriptCore API to users for maximum
  ///        flexibility. Each page has its own JSContext that gets reset
  ///        after each page navigation. You should make all your initial
  ///        JavaScript calls to the page within the DOMReady event,
  ///        @see ViewListener::OnDOMReady
  ///
  virtual JSContextRef js_context() = 0;

  ///
  /// Evaluate a raw string of JavaScript and return results as a native
  /// JavaScriptCore JSValueRef (@see <JavaScriptCore/JSValueRef.h>)
  ///
  /// @note  This is just a wrapper for JSEvaluateScript() in JavaScriptCore
  ///
  virtual JSValueRef EvaluateScript(const String& script) = 0;

  ///
  /// Whether or not we can navigate backwards in history
  ///
  virtual bool CanGoBack() = 0;

  ///
  /// Whether or not we can navigate forwards in history
  ///
  virtual bool CanGoForward() = 0;

  ///
  /// Navigate backwards in history
  ///
  virtual void GoBack() = 0;

  ///
  /// Navigate forwards in history
  ///
  virtual void GoForward() = 0;

  ///
  /// Navigate to an arbitrary offset in history
  ///
  virtual void GoToHistoryOffset(int offset) = 0;

  ///
  /// Reload current page
  ///
  virtual void Reload() = 0;

  ///
  /// Stop all page loads
  ///
  virtual void Stop() = 0;

  ///
  /// Give focus to the View.
  ///
  /// You should call this to give visual indication that the View has input
  /// focus (changes active text selection colors, for example).
  ///
  virtual void Focus() = 0;

  ///
  /// Remove focus from the View and unfocus any focused input elements.
  ///
  /// You should call this to give visual indication that the View has lost
  /// input focus.
  ///
  virtual void Unfocus() = 0;

  ///
  /// Whether or not the View has focus.
  ///
  virtual bool HasFocus() = 0;

  ///
  /// Whether or not the View has an input element with visible keyboard focus
  /// (indicated by a blinking caret).
  ///
  /// You can use this to decide whether or not the View should consume
  /// keyboard input events (useful in games with mixed UI and key handling).
  ///
  virtual bool HasInputFocus() = 0;
  
  ///
  /// Fire a keyboard event
  ///
  /// @note  Only 'Char' events actually generate text in input fields.
  ///
  virtual void FireKeyEvent(const KeyEvent& evt) = 0;

  ///
  /// Fire a mouse event
  ///
  virtual void FireMouseEvent(const MouseEvent& evt) = 0;

  ///
  /// Fire a scroll event
  ///
  virtual void FireScrollEvent(const ScrollEvent& evt) = 0;

  ///
  /// Set a ViewListener to receive callbacks for View-related events.
  ///
  /// @note  Ownership remains with the caller.
  ///
  virtual void set_view_listener(ViewListener* listener) = 0;
  
  ///
  /// Get the active ViewListener, if any
  ///
  virtual ViewListener* view_listener() const = 0;

  ///
  /// Set a LoadListener to receive callbacks for Load-related events.
  ///
  /// @note  Ownership remains with the caller.
  ///
  virtual void set_load_listener(LoadListener* listener) = 0;

  ///
  /// Get the active LoadListener, if any
  ///
  virtual LoadListener* load_listener() const = 0;

  ///
  /// Set whether or not this View should be repainted during the next
  /// call to Renderer::Render
  ///
  /// @note  This flag is automatically set whenever the page content changes
  ///        but you can set it directly in case you need to force a repaint.
  ///
  virtual void set_needs_paint(bool needs_paint) = 0;

  ///
  /// Whether or not this View should be repainted during the next call to
  /// Renderer::Render.
  ///
  virtual bool needs_paint() const = 0;

  ///
  /// Get the inspector for this View, this is useful for debugging and
  /// inspecting pages locally. This will only succeed if you have the
  /// inspector assets in your filesystem-- the inspector will look for
  /// file:///inspector/Main.html when it first loads.
  ///
  /// @note  The inspector View is owned by the View and lazily-created on
  ///        first call. The initial dimensions are 10x10, you should call
  ///        View::Resize() on the returned View to resize it to your desired
  ///        dimensions.
  ///
  virtual RefPtr<View> inspector() = 0;

protected:
  virtual ~View();
};

}  // namespace ultralight
