#ifndef URLGETCONTENT_H
#define URLGETCONTENT_H

#include "errorlog.h"
#include <QByteArray>
#include <QString>
#include <curl/curl.h>

class UrlGetContent {
	  public:
	UrlGetContent(const QByteArray& url, bool quiet, int category, int timeOut = 60, CURL* curl = nullptr);
	QByteArray execute(ErrorLog* eLog = nullptr);
	QString    sql;

	uint retryNum = 1;
	bool curlOk() const;

	CURLcode getCurlCode() const;
	long     getHttpCode() const;

	  private:
	QByteArray url;
	bool       quiet    = false;
	int        category = 0;
	int        timeOut  = 0;
	CURLcode   curlCode;
	long       httpCode      = 0;
	bool       callPerformed = false;
	CURL*      curl          = nullptr;
};

#endif // URLGETCONTENT_H
