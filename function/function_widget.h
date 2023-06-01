#ifndef FUNCTION_WIDGET_H
#define FUNCTION_WIDGET_H

#include <QNetworkReply>
#include <QMessageBox>

class FunctionWidget
{
public:
    FunctionWidget() {}
    virtual ~FunctionWidget() {}

    virtual void refresh() = 0;

protected:
    inline bool handleStatusCode(QNetworkReply *pReply)
    {
        int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute)
                             .toInt();
        QMessageBox msgBox;

        if (statusCode == 200 || statusCode == 201) {
            return true;
        } else if (statusCode == 429) {
            msgBox.setText("API 요청 횟수 제한 - 1분뒤 재시도해주세요.");
            msgBox.exec();
            return false;
        } else if (statusCode == 503) {
            msgBox.setText("로스트아크 서버 점검중");
            msgBox.exec();
            return false;
        } else {
            msgBox.setText(QString::number(statusCode));
            msgBox.exec();
            return false;
        }
    }
};

#endif // FUNCTION_WIDGET_H
