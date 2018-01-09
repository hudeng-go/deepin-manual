/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "view/web_window.h"

#include <DTitlebar>
#include <QDebug>
#include <QFileInfo>
#include <QResizeEvent>
#include <QTimer>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineView>

#include "base/consts.h"
#include "controller/search_manager.h"
#include "view/image_viewer_proxy.h"
#include "view/widget/image_viewer.h"
#include "view/widget/search_completion_window.h"
#include "view/search_proxy.h"
#include "view/title_bar_proxy.h"
#include "view/widget/title_bar.h"

namespace dman {

WebWindow::WebWindow(SearchManager* search_manager, QWidget* parent)
    : Dtk::Widget::DMainWindow(parent),
      app_name_(),
      search_manager_(search_manager),
      search_proxy_(new SearchProxy(search_manager, this)) {

  this->initUI();
  this->initConnections();
}

WebWindow::~WebWindow() {
}

void WebWindow::setAppName(const QString& app_name) {
  app_name_ = app_name;

  const QFileInfo info(kIndexPage);
  web_view_->load(QUrl::fromLocalFile(info.absoluteFilePath()));
}

void WebWindow::initConnections() {
  connect(title_bar_, &TitleBar::searchTextChanged,
          this, &WebWindow::onSearchTextChanged);
  connect(web_view_->page(), &QWebEnginePage::loadFinished,
          this, &WebWindow::onWebPageLoadFinished);
  connect(title_bar_, &TitleBar::upKeyPressed,
          completion_window_, &SearchCompletionWindow::goUp);
  connect(title_bar_, &TitleBar::downKeyPressed,
          completion_window_, &SearchCompletionWindow::goDown);
  connect(title_bar_, &TitleBar::focusOut,
          this, &WebWindow::onSearchEditFocusOut);

  connect(search_manager_, &SearchManager::searchResult,
          this, &WebWindow::onSearchResult);
}

void WebWindow::initUI() {
  completion_window_ = new SearchCompletionWindow();

  title_bar_ = new TitleBar();
  title_bar_proxy_ = new TitleBarProxy(title_bar_, this);
  this->titlebar()->setCustomWidget(title_bar_, Qt::AlignCenter, false);
  this->titlebar()->setSeparatorVisible(true);

  image_viewer_ = new ImageViewer(this);
  image_viewer_proxy_ = new ImageViewerProxy(image_viewer_, this);

  web_view_ = new QWebEngineView();
  this->setCentralWidget(web_view_);

  // Use TitleBarProxy instead.
  QWebChannel* channel = new QWebChannel(this);
  web_view_->page()->setWebChannel(channel);
  channel->registerObject("imageViewer", image_viewer_proxy_);
  channel->registerObject("search", search_proxy_);
  channel->registerObject("titleBar", title_bar_proxy_);

  this->setFocusPolicy(Qt::ClickFocus);
}

void WebWindow::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  title_bar_->setFixedWidth(event->size().width());
}

void WebWindow::onSearchEditFocusOut() {
  qDebug() << "Focus out";
  QTimer::singleShot(50, [=]() {
    this->completion_window_->hide();
  });
}

void WebWindow::onSearchTextChanged(const QString& text) {
  if (text.size() > 1 && completion_window_ != nullptr) {
    // Do real search.
    completion_window_->setKeyword(text);
    completion_window_->show();
    // Move to bottom of search edit.
    const QRect geom = this->geometry();
    completion_window_->move(geom.x() + this->width() / 2, geom.y());
    completion_window_->setFocusPolicy(Qt::StrongFocus);
    completion_window_->resize(200, 400);
    completion_window_->raise();
    search_manager_->search(search_proxy_->currentApp(), text);
  }
}

void WebWindow::onWebPageLoadFinished(bool ok) {
  if (ok) {
//    web_view_->page()->runJavaScript(
//        "function getSystemManualDir() { return '/usr/share/dman'; }");
    if (app_name_.isEmpty()) {
      web_view_->page()->runJavaScript("index()");
    } else {
      QString real_path(app_name_);
      if (real_path.contains('/')) {
        // Open markdown file with absolute path.
        QFileInfo info(real_path);
        real_path = info.canonicalFilePath();
        web_view_->page()->runJavaScript(
            QString("openFile(\"%1\")").arg(real_path));
      } else {
        // Open system manual.
        web_view_->page()->runJavaScript(
            QString("openApp(\"%1\")").arg(app_name_));
      }
    }
  }
}

void WebWindow::onSearchResult(const QString& app_name,
                               const QString& keyword,
                               const SearchResultList& result) {
  Q_UNUSED(app_name);
  Q_UNUSED(keyword);
  Q_UNUSED(result);
  if (result.isEmpty()) {
    // Show empty search result in web.
  } else {
    completion_window_->setResult(result);
  }
}

}  // namespace dman