#ifndef SCRIPTTHREAD_HPP
#define SCRIPTTHREAD_HPP
#include <QScriptEngineDebugger>
#include <QtScript/QtScript>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptable>
#include <unistd.h>
class ScriptThread : public QThread
{
private:
    QScriptEngine JSEngine;
    QScriptValue result;
    QString JSCode;
    QTextEdit textBrowser;
    void run()
    {
        new Q_DebugStream(std::cout, &textBrowser); //Redirect Console output to QTextEdit
        Q_DebugStream::registerQDebugMessageHandler(); //Redirect qDebug() output to QTextEdit

        result = JSEngine.evaluate(JSCode);
        sleep(5);
    }
public:
    QString getOutput()
    {
        QString output = textBrowser.document()->toPlainText();
        textBrowser.clear();
        return output;
    }
    QScriptEngine* getEngine()
    {
        return &JSEngine;
    }
    void setCode(QString code)
    {
        JSCode = code;
    }
    QScriptValue getResult()
    {
        return result;
    }
};

#endif // SCRIPTTHREAD_HPP
