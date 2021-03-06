#include "SocketHandler.h"

#include <iostream>

using namespace std;

SocketHandler::SocketHandler(QTcpSocket *sock, QScriptEngine *engine, QtScriptPlugin *plugin)
{
    clientSocket = sock;
    this->engine = engine;

    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(echo()));
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(killMe()));

    cout << "QtScript console started, connect with \"telnet <kobo ip> 1337\"" << endl;
    sock->write("Welcome to the Kobo QtScript console!\n> ");
    connect(this, SIGNAL(open(QString)), plugin, SLOT(open(QString)), Qt::QueuedConnection);
}

void SocketHandler::echo()
{
    QString request, tmprequest, response;
    while (clientSocket->canReadLine())
    {
        tmprequest = clientSocket->readLine();
        request += tmprequest;
    }

    if (request.startsWith("quit")) {
        clientSocket->write("bye\n");
        clientSocket->waitForBytesWritten(1000);
        clientSocket->disconnectFromHost();
    }
    if (request.startsWith("open")) {
        emit open("application/x-games-Sudoku");
    } else {
        QString result = engine->evaluate(request).toString();
        response = "result: " + result + "\n> ";
        clientSocket->write(response.toAscii());
    }
}

void SocketHandler::killMe(){
    if (this != NULL) {
        cout << "Killing the SocketHandler!" << endl;
        delete this;
    }
}
