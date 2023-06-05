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
};

#endif // FUNCTION_WIDGET_H
