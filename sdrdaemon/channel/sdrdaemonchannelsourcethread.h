///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Edouard Griffiths, F4EXB.                                  //
//                                                                               //
// SDRdaemon source channel (Tx) UDP receiver thread                             //
//                                                                               //
// SDRdaemon is a detached SDR front end that handles the interface with a       //
// physical device and sends or receives the I/Q samples stream to or from a     //
// SDRangel instance via UDP. It is controlled via a Web REST API.               //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#ifndef SDRDAEMON_CHANNEL_SDRDAEMONCHANNELSOURCETHREAD_H_
#define SDRDAEMON_CHANNEL_SDRDAEMONCHANNELSOURCETHREAD_H_

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QHostAddress>

#include "util/message.h"
#include "util/messagequeue.h"

class SDRDaemonDataQueue;
class SDRDaemonDataBlock;
class QUdpSocket;

class SDRDaemonChannelSourceThread : public QThread {
    Q_OBJECT
public:
    class MsgStartStop : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        bool getStartStop() const { return m_startStop; }

        static MsgStartStop* create(bool startStop) {
            return new MsgStartStop(startStop);
        }

    protected:
        bool m_startStop;

        MsgStartStop(bool startStop) :
            Message(),
            m_startStop(startStop)
        { }
    };

    class MsgDataBind : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        QHostAddress getAddress() const { return m_address; }
        uint16_t getPort() const { return m_port; }

        static MsgDataBind* create(const QString& address, uint16_t port) {
            return new MsgDataBind(address, port);
        }

    protected:
        QHostAddress m_address;
        uint16_t m_port;

        MsgDataBind(const QString& address, uint16_t port) :
            Message(),
            m_port(port)
        {
            m_address.setAddress(address);
        }
    };

    SDRDaemonChannelSourceThread(SDRDaemonDataQueue *dataQueue, QObject* parent = 0);
    ~SDRDaemonChannelSourceThread();

    void startStop(bool start);
    void dataBind(const QString& address, uint16_t port);

private:
    QMutex m_startWaitMutex;
    QWaitCondition m_startWaiter;
    bool m_running;

    MessageQueue m_inputMessageQueue;
    SDRDaemonDataQueue *m_dataQueue;

    QHostAddress m_address;
    QUdpSocket *m_socket;

    static const uint32_t m_nbDataBlocks = 4;          //!< number of data blocks in the ring buffer
    SDRDaemonDataBlock *m_dataBlocks[m_nbDataBlocks];  //!< ring buffer of data blocks indexed by frame affinity

    void startWork();
    void stopWork();

    void run();

private slots:
    void handleInputMessages();
    void readPendingDatagrams();
};



#endif /* SDRDAEMON_CHANNEL_SDRDAEMONCHANNELSOURCETHREAD_H_ */
