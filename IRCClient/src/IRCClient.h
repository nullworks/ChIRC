/*
 * Copyright (C) 2011 Fredi Machado <https://github.com/fredimachado>
 * IRCClient is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * http://www.gnu.org/licenses/lgpl.html 
 */

#ifndef _IRCCLIENT_H
#define _IRCCLIENT_H

#include <string>
#include <vector>
#include <list>
#include "IRCSocket.h"
#include <functional>

class IRCClient;

extern std::vector<std::string> split(std::string const &, char);

struct IRCCommandPrefix
{
    void Parse(std::string data)
    {
        if (data == "")
            return;

        prefix = data.substr(1, data.find(" ") - 1);
        std::vector<std::string> tokens;

        if (prefix.find("@") != std::string::npos)
        {
            tokens = split(prefix, '@');
            nick   = tokens.at(0);
            host   = tokens.at(1);
        }
        if (nick != "" && nick.find("!") != std::string::npos)
        {
            tokens = split(nick, '!');
            nick   = tokens.at(0);
            user   = tokens.at(1);
        }
    };

    std::string prefix;
    std::string nick;
    std::string user;
    std::string host;
};

struct IRCMessage
{
    IRCMessage();
    IRCMessage(std::string cmd, IRCCommandPrefix p, std::vector<std::string> params)
        : command(cmd), prefix(p), parameters(params){};

    std::string command;
    IRCCommandPrefix prefix;
    std::vector<std::string> parameters;
};

struct IRCCommandHook
{
    IRCCommandHook()
        : function(NULL){};

    std::string command;
    std::function<void(IRCMessage /*message*/, IRCClient * /*client*/, void *context)> function;
    // A ptr that can be used for whatever
    void *context;
};

class IRCClient
{
public:
    IRCClient()
        : _debug(false){};

    bool InitSocket();
    bool Connect(const char * /*host*/, int /*port*/);
    void Disconnect();
    bool Connected()
    {
        return _socket.Connected();
    };

    bool SendIRC(std::string /*data*/);

    bool Login(std::string /*nick*/, std::string /*user*/, std::string /*password*/ = std::string());

    void ReceiveData();

    void HookIRCCommand(std::string command, void *context /*ptr for whatever*/, std::function<void(IRCMessage /*message*/, IRCClient * /*client*/, void *context /*ptr for whatever*/)> function);

    void Parse(std::string /*data*/);

    void HandleCTCP(IRCMessage /*message*/);

    // Default internal handlers
    void HandlePrivMsg(IRCMessage /*message*/);
    void HandleNotice(IRCMessage /*message*/);
    void HandleChannelJoinPart(IRCMessage /*message*/);
    void HandleUserNickChange(IRCMessage /*message*/);
    void HandleUserQuit(IRCMessage /*message*/);
    void HandleChannelNamesList(IRCMessage /*message*/);
    void HandleNicknameInUse(IRCMessage /*message*/);
    void HandleServerMessage(IRCMessage /*message*/);

    void Debug(bool debug)
    {
        _debug = debug;
    };

private:
    void HandleCommand(IRCMessage /*message*/);
    void CallHook(std::string /*command*/, IRCMessage /*message*/);

    IRCSocket _socket;

    std::list<IRCCommandHook> _hooks;

    std::string _nick;
    std::string _user;

    bool _debug;
};

#endif
