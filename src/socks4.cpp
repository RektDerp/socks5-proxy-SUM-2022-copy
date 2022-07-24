#include "socks4.h"
#include "session.h"
#include "string_utils.h"

bool socks4::init()
{
    /*
                 +----+----+----+----+----+----+----+----+----+----+....+----+
		         | VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
		         +----+----+----+----+----+----+----+----+----+----+....+----+
      # of bytes:  1    1      2              4           variable       1
    */

    bs::error_code ec;
    int method = _session->readByte(ec);
    if (method != CMD::CONNECT) {
        log(ERROR_LOG) << "[socks4] Method is not supported: " << method;
        return false;
    }
    readPort();
    _dstAddress = readIPV4();
    if (_dstAddress.length() == 0) return false;
    bvec userid;
    ec.clear();
    while (true) {
        unsigned char c = _session->readByte(ec);
        if (c && !ec) userid.push_back(c);
        else break;
    }
    if (checkError(ec))
        return false;
    _username = string_utils::to_string(userid);

    ba::ip::tcp::resolver::query query(_dstAddress, std::to_string(_serverPort));

    /*
                +----+----+----+----+----+----+----+----+
                | VN | CD | DSTPORT |      DSTIP        |
                +----+----+----+----+----+----+----+----+
 # of bytes:	   1    1      2              4
    
    */

    _bindPort = _session->connect(query, ec);
    bvec reply;
    reply.resize(8);
    reply[1] = ec ? REJECTED : GRANTED;
    _session->writeBytes(reply, ec);
    if (checkError(ec) || reply[1] == REJECTED) {
        return false;
    }

    if (!createRecord())
        return false;
    return true;
}