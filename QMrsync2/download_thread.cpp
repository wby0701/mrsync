#include <rsync/rsync_client.h>
#include <rsync/rsync_socketio.h>
#include <rsync/rsync_socketutil.h>
#include <rsync/rsync_pathutil.h>
#include "download_thread.h"

namespace rsync
{
	DownloadThread::DownloadThread( const char *server, const char *usr,
									const char* password, std::string remoteDir,
									std::string localDir, const char *module, int cancelFlag)
		: d_server(server)
		, d_usr(usr)
		, d_password(password)
		, d_remoteDir(remoteDir)
		, d_localDir(localDir)
		, d_module(module)
		, d_cancelFlag(cancelFlag)
	{
	}
	DownloadThread::~DownloadThread()
	{
		wait();
	}

	void DownloadThread::createLocalDir(std::string &remoteDir, std::string &localDir)
	{
		size_t begin_pos = remoteDir.find_first_of("/");
		size_t end_pos = remoteDir.find_last_of("/");
		localDir = localDir + remoteDir.substr(begin_pos, end_pos - begin_pos);
	}


	void DownloadThread::run()
	{
		createLocalDir(d_remoteDir, d_localDir);
		SocketIO socketIO;
		socketIO.connect(d_server, 873, d_usr, d_password, d_module);
		Client client(&socketIO, "rsync", 30, &d_cancelFlag);
		char buff[20];
		itoa(reinterpret_cast<uint>(QThread::currentThreadId()), buff, 16);
		std::string  temporaryFile = PathUtil::join(d_localDir.c_str(), std::string(buff).append("acrosync.part").c_str());	
		client.download(d_localDir.c_str(), d_remoteDir.c_str(), temporaryFile.c_str(), 0);
	}

}
