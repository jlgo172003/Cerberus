/*
Copyright (C) 2009 James Choa

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <iostream>
#include "net/contestant_connection.h"
#include "util/sql_util.h"
#include "error_defs.h"

using namespace std;

ContestantConnection::ContestantConnection ( QObject* parent, QTcpSocket* socket ) : QObject ( parent )
{
        m_hdr = NULL;
        m_authenticated = false;
        m_socket = socket;
        connect ( m_socket, SIGNAL ( error ( QAbstractSocket::SocketError ) ),
                  this, SLOT ( error ( QAbstractSocket::SocketError ) ) );
        connect ( m_socket, SIGNAL ( readyRead() ), this, SLOT ( ready() ) );
        connect ( m_socket, SIGNAL ( disconnected() ), this, SLOT ( disconnected() ) );
        // reply to client of the now established connection
        QByteArray block;
        QDataStream out ( &block, QIODevice::WriteOnly );
        out.setVersion ( QDataStream::Qt_4_5 );
        // construct the header
        p_header hdr;
        hdr.length = sizeof ( ushort );
        hdr.command = NET_CONNECTION_RESULT;

        out.writeRawData ( ( const char* ) &hdr, sizeof ( p_header ) );
        out << ( ushort ) true;

        m_socket->write ( block );
}

void ContestantConnection::error ( const QAbstractSocket::SocketError& err )
{
        //do something about the error here
}

void ContestantConnection::ready()
{
        //read the socket data
        QDataStream in ( m_socket );
        in.setVersion ( QDataStream::Qt_4_5 );
        if ( m_hdr == NULL ) {
                if ( m_socket->bytesAvailable() < ( int ) sizeof ( p_header ) ) {
                        return;
                }
                m_hdr = new p_header;
                in.readRawData ( ( char* ) m_hdr, sizeof ( p_header ) );
                //check the packet
                if ( strcmp ( ( const char* ) m_hdr->ident.data, "CERB" ) != 0 ) {
                        // bad packet, do something here
                        return;
                }
                //check the version
                if ( !is_proto_current ( m_hdr->ver ) ) {
                        // the version is not the same, do something here
                }
        }
        if ( m_socket->bytesAvailable() < m_hdr->length ) {
                return;
        }
        switch ( m_hdr->command ) {
        case QRY_CONTEST_STATE:
                //contestant is asking for the contest state.
                if ( m_authenticated ) {
                        sendContestState();
                } else {
                        //send an error
                        errorReply ( ERR_NOTAUTHORIZED );
                }
                break;
        case QRY_AUTHENTICATE:
                //contestant wants to authenticate
                if ( !m_authenticated ) {
                        QString buffer, user, pass;
                        in >> buffer;
                        user = buffer.section ( ",", 0, 0 );
                        // don't split, the password is hashed and may contain ','
                        pass = buffer.right ( buffer.size()-user.size()-1 );
                        bool result = SqlUtil::getInstance().authenticate ( user, pass );
                        authenticationReply ( result );
                        m_authenticated = result;
                } else {
                        //TODO: what happens here?
                        authenticationReply ( false );
                }
                break;
        case QRY_QUESTION_REQUEST:
                //contestant is asking for question data
                ushort round;
                in >> round;
                if ( m_authenticated ) {
                        sendQData ( m_qdata->at ( round-1 ) );
                } else {
                        //send an error
                        errorReply ( ERR_NOTAUTHORIZED );
                }
                break;
        case QRY_ANSWER_SUBMIT:
                //contestant has submitted their answers
                if ( m_authenticated ) {
                        QString xml;
                        uchar hash[20];
                        in.readRawData ( ( char* ) hash, 20 );
                        in >> xml;
                        QByteArray testhash = QCryptographicHash::hash ( xml.toAscii(), QCryptographicHash::Sha1 );
                        QByteArray testhash2;
                        for ( int i = 0; i < 20; i++ ) {
                                testhash2.push_back ( hash[i] );
                        }
                        if ( testhash == testhash2 ) {
                                // TODO: insert code for checker here
                                sendAReply ( true );
                        } else {
                                sendAReply ( false );
                        }
                } else {
                        errorReply ( ERR_NOTAUTHORIZED );
                }
                break;
        default:
                ;
                // invalid command, do something here
        }
        delete m_hdr;
        m_hdr = NULL;
}

void ContestantConnection::errorReply ( ERROR_MESSAGES err )
{
        //construct the packet and send it
        QByteArray block;
        QDataStream out ( &block, QIODevice::WriteOnly );
        out.setVersion ( QDataStream::Qt_4_5 );
        // construct the header
        p_header hdr;
        hdr.length = sizeof ( uchar );
        hdr.command = INF_ERROR;

        out.writeRawData ( ( const char* ) &hdr, sizeof ( p_header ) );
        out << ( uchar ) err;

        m_socket->write ( block );
}

void ContestantConnection::disconnected()
{
        //do something about our disconnection here
        emit contestantDisconnect ( this );
}

void ContestantConnection::authenticationReply ( bool res )
{
        //construct the packet and send it
        QByteArray block;
        QDataStream out ( &block, QIODevice::WriteOnly );
        out.setVersion ( QDataStream::Qt_4_5 );
        // construct the header
        p_header hdr;
        hdr.length = sizeof ( uchar );
        hdr.command = INF_AUTHENTICATE;

        out.writeRawData ( ( const char* ) &hdr, sizeof ( p_header ) );
        out << ( uchar ) res;

        m_socket->write ( block );
}

void ContestantConnection::sendQData ( const QString& xml )
{
        //construct the packet and send it
        QByteArray block;
        QDataStream out ( &block, QIODevice::WriteOnly );
        out.setVersion ( QDataStream::Qt_4_5 );
        // construct the header
        p_header hdr;
        hdr.command = INF_QUESTION_DATA;
        QByteArray hash = QCryptographicHash::hash ( xml.toAscii(), QCryptographicHash::Sha1 );
        hdr.length = hash.size() +xml.size();
        out.writeRawData ( ( const char* ) &hdr, sizeof ( p_header ) );
        out.writeRawData ( hash.data(), hash.size() );
        out << xml;
        m_socket->write ( block );
}

void ContestantConnection::sendAReply ( bool res )
{
        //construct the packet and send it
        QByteArray block;
        QDataStream out ( &block, QIODevice::WriteOnly );
        out.setVersion ( QDataStream::Qt_4_5 );
        // construct the header
        p_header hdr;
        hdr.command = INF_ANSWER_REPLY;
        hdr.length = sizeof ( uchar );
        out.writeRawData ( ( const char* ) &hdr, sizeof ( p_header ) );
        out << ( uchar ) res;
        m_socket->write ( block );
}

void ContestantConnection::sendContestState ()
{
        QByteArray block;
        QDataStream out ( &block, QIODevice::WriteOnly );
        out.setVersion ( QDataStream::Qt_4_5 );
        // construct the header
        p_header hdr;
        hdr.length = sizeof ( ushort ) + sizeof ( uchar );
        hdr.command = INF_CONTEST_STATE;

        out.writeRawData ( ( const char* ) &hdr, sizeof ( p_header ) );
        out << ( ushort ) m_round << ( uchar ) m_con_status;

        m_socket->write ( block );
}

void ContestantConnection::setRound ( int round )
{
        m_round = round;
}

void ContestantConnection::setStatus ( CONTEST_STATUS s )
{
        m_con_status = s;
}
