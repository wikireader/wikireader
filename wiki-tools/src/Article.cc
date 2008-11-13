/*
 * Wiki Handling Tool
 *
 * Copyright (C) 2008 Openmoko Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Article.h"
#include "sha1.h"

#include <QApplication>
#include <QWebFrame>
#include <QWebView>


// Todo: For the final conversion of the data switch to a plain QWebPage
// currently we require a gui.
class ArticleFetcher : public QObject {
    Q_OBJECT
public:
    ArticleFetcher();
    ~ArticleFetcher();
    bool fetch(const QString& title);

    QString toHtml();
    QString toPlainText();

private Q_SLOTS:
    void loadFinished(bool);

private:
    QWebView* m_webView;
    bool m_done;
    bool m_result;
};

ArticleFetcher::ArticleFetcher()
    : m_webView(0)
    , m_done(false)
    , m_result(false)
{}

ArticleFetcher::~ArticleFetcher()
{
    delete m_webView;
}

bool ArticleFetcher::fetch(const QString& title)
{
    if (!m_webView) {
        QWebPage* page = new QWebPage(this);
        m_webView = new QWebView(0);
        m_webView->setGeometry(0, 0, 640, 480);
        m_webView->setPage(page);
        connect(m_webView, SIGNAL(loadFinished(bool)),
                SLOT(loadFinished(bool)));
    }


    m_done = false;
    m_result = false;

    // hopefully no loadFinished is fired on replacing one page with another one
    m_webView->setUrl(title);

    // Evil loop
    while (!m_done) {
        QApplication::processEvents();
    }

    return m_result;
}

void ArticleFetcher::loadFinished(bool result)
{
    m_result = result;
    m_done = true;
}

QString ArticleFetcher::toHtml()
{
    Q_ASSERT(m_webView);

    return m_webView->page()->mainFrame()->toHtml();
}

QString ArticleFetcher::toPlainText()
{
    Q_ASSERT(m_webView);

    return m_webView->page()->mainFrame()->toPlainText();
}

Article::Article()
    : m_isEmpty(true)
{}

Article::Article(const Title& title, const QString& textContent)
    : m_title(title)
    , m_textContent(textContent)
    , m_isEmpty(false)
    , m_isRedirect(false)
    , m_determinedRedirect(false)
{}

bool Article::isEmpty() const
{
    return m_isEmpty;
}

QString Article::redirectsTo() const
{
    return m_redirectsTo;
}

bool Article::isRedirect() const
{
    if (m_determinedRedirect)
        return m_isRedirect;

    // in Python notation re.compile("^\#REDIRECT \[\[(?P<title>[a-zA-Z0-9- \t]*)\]\]$")
    // and done manually here. This means the string is walked multiple times but only
    // in the case of a #REDIRECT.
    m_determinedRedirect = true;

    if (!m_textContent.startsWith("#REDIRECT [["))     
        return m_isRedirect;

    if (m_textContent.contains("\n"))
        return m_isRedirect;

    if (!m_textContent.contains("]]"))
        return m_isRedirect;

    m_isRedirect = true;
    m_redirectsTo = m_textContent.mid(sizeof "#REDIRECT [[" - 1);
    m_redirectsTo = m_redirectsTo.left(m_redirectsTo.indexOf("]]"));
    return m_isRedirect;
}

Title Article::title() const
{
    return m_title;
}

QString Article::textContent() const
{
    return m_textContent;
}

QByteArray Article::hash() const
{
    // build sha1 hash of the text content
    if (!m_result.isEmpty())
        return m_result;

    SHA_CTX ctx;
    SHA1_Init(&ctx);

    QByteArray utf8 = m_textContent.toUtf8();
    QByteArray result;
    result.resize(20);
    SHA1_Update(&ctx, utf8.constData(), utf8.length());
    SHA1_Final((unsigned char*)result.data(), &ctx);

    m_result = result.toHex();
    return m_result;
}

QString Article::toPlainText() const
{
    // Assume the html is not empty when we fetched it
    if (!m_htmlText.isEmpty())
        return m_plainText;

    toHtml();

    return m_plainText;
}

QString Article::toHtml() const
{
    if (!m_htmlText.isEmpty())
        return m_htmlText;

    // Spawn loading of a QWebView... and get the html/text from it
    ArticleFetcher fetcher;
    bool result = fetcher.fetch(QString::fromLatin1("http://127.0.0.1/mediawiki/index.php/%1").arg(m_title.title()));

    if (!result)
        return m_htmlText;

    m_htmlText = fetcher.toHtml();
    m_plainText = fetcher.toPlainText();

    return m_htmlText;
}

#include "Article.moc"
