/***************************************************************************
 *
 *  Project
 *
 * Copyright (C) 2013 - 2013, Paul Zhou, <qianlong.zhou@gmail.com>.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#ifndef ISIPSTACK_H_INCLUDED
#define ISIPSTACK_H_INCLUDED

#include <string>

/**
 * SIP传输协议
 */
enum SipProtocol {
    /** UDP协议 */
    SIP_UDP,
    /** TCP协议 */
    SIP_TCP,
    /** DTLS-UDP 协议 */
    SIP_DTLS,
    /** TLS 协议 */
    SIP_TLS
};

/**
 * @brief 媒体数据传输信息 (地址，端口)
 */
class TransportInfo {
public:
    /**
     * 构造函数
     * @param[in] a_addr 语音数据地址
     * @param[in] a_port 语音数据端口号
     * @param[in] v_addr 视频数据地址
     * @param[in] v_port 视频数据端口号
     */
    TransportInfo(const char* a_addr, unsigned short a_port,
                  const char* v_addr, unsigned short v_port) {
        this->audio_addr = a_addr;
        this->audio_port = a_port;
        this->video_addr = v_addr;
        this->video_port = v_port;
    }
public:
    /** 音频数据地址 */
    std::string audio_addr;
    /** 视频数据地址 */
    std::string video_addr;
    /** 音频数据端口 */
    unsigned short audio_port;
    /** 视频数据端口 */
    unsigned short video_port;
};

/**
 * @brief SIP事件处理接口 （回调)
 */
class ISipEventHander {
public:
    virtual ~ISipEventHander(){}
     /**
     * 登录结果
     * @param[in] success 登录是否成功
     * @param[in] err  登录的错误信息
     */
    virtual void OnLogin(bool success, const char* err=NULL)=0;
     /**
     * 注销完成
     * @param[in] msg 注销信息
     */
    virtual void OnLogout(const char* msg=NULL)=0;
     /**
     * 来电
     * @param[in] username 用户名
     * @param[in] sessionid 会话ID
     * @param[in] TransportInfo& transport_info
     */
    virtual void OnIncoming(const char* username, long sessionid, TransportInfo& transport_info)=0;
    /**
     * 已接通
     * @param[in] sessionid 会话ID
     */
    virtual void OnConnected(long sessionid)=0;
    /**
     * 会话结束
     * @param[in] sessionid 会话ID
     */
    virtual void OnClose(long sessionid)=0;
    /**
     * 对方正在振铃
     * @param[in] sessionid 会话ID
     */
    virtual void OnRinging(long sessionid)=0;
};

// 定义SipStack标识
APF_DECLARE_CLASSID(CLSID_SipStack, "SipStack")

/**
 * @brief SIP协议栈接口
 * SIP的基本操作（注册，呼叫，接受，拒绝等)
 */
class ISipStack {
APF_DECLARE_INTERFACE(ISipStack)
public:
    virtual ~ISipStack(){}
    /**
     * 初始化SIP协议栈
     * @param[in] protocol SIP所使用的传输协议（UDP、TCP)
     * @param[in] port  监听的socket端口，若是TCP协议则不用设置
     * @return 初始化是否成功
     *
     */
    virtual bool Init(SipProtocol protocol, unsigned short port=0)=0;

    /**
     * 释放SIP协议栈
     *
     */
    virtual void Uninit()=0;

    /**
     * 设置支持的编码格式
     * @param[in] codec 编码格式
     * @example sip->AddAudioCodec("10 PCMA/8000");
     */
     virtual void AddAudioCodec(const char* codec)=0;

    /**
     * 设置支持的编码格式
     * @param[in] codec 编码格式
     */
     virtual void AddVideoCodec(const char* codec)=0;

     /**
      * 清除编码格式信息
      */
      virtual void ClearCodec()=0;

    /**
     * 登录到代理服务器
     * @param[in] username 用户名
     * @param[in] password 密码
     * @param[in] host 代理服务器地址
     * @param[in] port 代码服务器端口
     * @return
     * @note 登录的状态将在回调中返回
     */
    virtual void Login(const char* username, const char* password, const char* host, unsigned short port)=0;

    /**
     * 从代理服务器注销
     * @note 登录的状态将在回调中返回
     */
    virtual void Logout()=0;

    /**
     * 呼叫请求
     * @param[in] username 用户名
     * @param[in] transport_info 网络传输信息
     * @return 会话 id
     */
    virtual long Invite(const char* username, TransportInfo& transport_info)=0;

    /**
     * 接受呼叫请求
     * @param[in] sessionid 会话ID
     * @param[in] transport_info 网络传输信息
     */
    virtual void Accept(long sessionid, TransportInfo& transport_info)=0;

    /**
     * 拒绝呼叫请求
     * @param[in] sessionid 会话ID
     */
    virtual void Reject(long sessionid)=0;

    /**
     * 结束会话
     * @param[in] sessionid 会话ID
     */
     virtual void Close(long sessionid)=0;

    /**
     * 设置事件监听对象
     * @param[in] handler 事件监听对象
     */
     virtual void SetEventHandler(ISipEventHander* handler)=0;
};

#endif // ISIPSTACK_H_INCLUDED
