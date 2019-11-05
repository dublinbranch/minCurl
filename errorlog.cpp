#include "errorlog.h"
#include <QDateTime>
#include <QDebug>

int     ErrorLog::truncatedResponseLength = 100;
QString ErrorLog::db                      = "db";
QString ErrorLog::table                   = "table";

QString base64this_copy(const QByteArray& param) {
	return "FROM_BASE64('" + param.toBase64() + "')";
}

QString ErrorLog::logQuery(const curlCall* call) {
	auto       curl     = call->curl;
	auto       response = call->response;
	auto       get      = call->get;
	auto       post     = call->post;
	// milliseconds
	qint64     now      = QDateTime::currentMSecsSinceEpoch();
	CURLTiming timing   = curlTimer(curl);
	// seconds
	double totalTime   = timing.totalTime;
	double preTransfer = timing.preTransfer;

	long httpCode;
	auto res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
	if (res != CURLE_OK) {
		qCritical().noquote() << "curl_easy_getinfo() didn't return the curl code.\n";
	}

	QByteArray truncatedResp = response.left(truncatedResponseLength);

	QByteArray sErrBuf;
	if (call->errbuf[0] == '\0') {
		sErrBuf = "NULL";
	} else {
		sErrBuf = call->errbuf;
	}

	// query
	static const QString skel = R"EOD(
	INSERT INTO %1.%2 SET
	ts = %3,
	totalTime = %4,
	preTransfer = %5,
	curlCode = %6,
	httpCode = %7,
	get = %8,
	post = %9,
	response = %10,
	errBuf = %11,
	category = %12
)EOD";

	auto sql = skel.arg(db)
	               .arg(table)
	               .arg(now)
	               .arg(totalTime)
	               .arg(preTransfer)
	               .arg(call->curlCode)
	               .arg(httpCode)
	               .arg(base64this_copy(get))
	               .arg(base64this_copy(post))
	               .arg(base64this_copy(truncatedResp))
	               .arg(base64this_copy(sErrBuf))
	               .arg(call->category);

	return sql;
}
