// Author: Sergey Linev <S.Linev@gsi.de>
// Date: 2017-06-29
// Warning: This is part of the ROOT 7 prototype! It will change without notice. It might trigger earthquakes. Feedback is welcome!

/*************************************************************************
 * Copyright (C) 1995-2023, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_cef_gui_handler_h
#define ROOT_cef_gui_handler_h

#include "include/cef_client.h"
#include "include/base/cef_ref_counted.h"
#include "include/cef_version.h"
#include "include/wrapper/cef_resource_manager.h"
#include <list>
#include <vector>

namespace ROOT {
class RLogChannel;
}

ROOT::RLogChannel &CefWebDisplayLog();

class THttpServer;

/** \class GuiHandler
\ingroup cefwebdisplay
*/

class GuiHandler : public CefClient,
                   public CefLifeSpanHandler,
                   public CefLoadHandler,
                   public CefDisplayHandler,
                   public CefRequestHandler,
                   public CefResourceRequestHandler {
protected:

   bool fUseViews{false};   ///<! if view framework is used, required for true headless mode
   int fConsole{0};         ///<! console parameter, assigned via WebGui.Console rootrc parameter
   typedef std::list<CefRefPtr<CefBrowser>> BrowserList; ///<! List of existing browser windows. Only accessed on the CEF UI thread.
   BrowserList fBrowserList;

   std::vector<THttpServer *> fServers;

   bool is_closing_{false};

public:
   explicit GuiHandler(bool use_views = false);

   // Provide access to the single global instance of this object.
   // static BaseHandler *GetInstance();

   // CefClient methods:
   CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
   CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
   CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
   CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

   // CefLifeSpanHandler methods:
   void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
   bool DoClose(CefRefPtr<CefBrowser> browser) override;
   void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

   // CefLoadHandler methods:
   void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode,
                    const CefString &errorText, const CefString &failedUrl) override;

   // CefDisplayHandler methods:
   void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title) override;

   bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                         cef_log_severity_t level,
                         const CefString &message, const CefString &source,
                         int line) override;

   // Request that all existing browser windows close.
   void CloseAllBrowsers(bool force_close);

   bool IsClosing() const { return is_closing_; }

   // CefRequestHandler methods:
   CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
         CefRefPtr<CefBrowser> browser,
         CefRefPtr<CefFrame> frame,
         CefRefPtr<CefRequest> request,
         bool is_navigation,
         bool is_download,
         const CefString& request_initiator,
         bool& disable_default_handling) override { return this; }

   // CefResourceRequestHandler methods:
   cef_return_value_t OnBeforeResourceLoad(
         CefRefPtr<CefBrowser> browser,
         CefRefPtr<CefFrame> frame,
         CefRefPtr<CefRequest> request,
         CefRefPtr<CefCallback> callback) override;

   CefRefPtr<CefResourceHandler> GetResourceHandler(
         CefRefPtr<CefBrowser> browser,
         CefRefPtr<CefFrame> frame,
         CefRefPtr<CefRequest> request) override;

   std::string AddBatchPage(const std::string &cont);

   std::string MakePageUrl(THttpServer *serv, const std::string &addr);

   static bool PlatformInit();

   static std::string GetDataURI(const std::string& data, const std::string& mime_type);

   static bool PlatformResize(CefRefPtr<CefBrowser> browser, int width, int height);

private:

   // Platform-specific implementation.
   void PlatformTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title);

   CefRefPtr<CefResourceManager> fResourceManager;
   int fBatchPageCount{0};

   // Include the default reference counting implementation.
   IMPLEMENT_REFCOUNTING(GuiHandler);
   DISALLOW_COPY_AND_ASSIGN(GuiHandler);
};


#endif // ROOT_cef_gui_handler_h
