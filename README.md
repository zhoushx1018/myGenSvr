

@brief myGenSvr, linux c\c++ server framework




 常用的 one event loop per thread 的 server框架
多线程读取客户端请求的 server框架
	
main
	主线程
	监听、accept，建立的 socket连接，通过fd哈希到特定SubReactor对象，并将连接移交至该SubReactor对象
	

SubReactor
	使用 Reactor模式
	每个SubReactor对象只有 一个 event loop ，一个 thread

	内部使用epoll 作为统一的io事件、定时事件的触发器；
	外部事件（新连接到来、发往客户端数据到来），通过管道注册到epoll，从而保证epoll事件的统一；

	队列
  		发往客户端数据队列；
  		通过管道读事件，触发该队列的读事件

	事件：
  		客户端读事件
  		管道读事件:客户端新连接的到来
  		管道读事件：发往客户端数据的到来
  		内部唤醒( 即epoll超时）
  
SvrSender
	主动对象，内部有待发送队列；
	发送数据到服务端，收到应答后，哈希到特定 SubReactor对象，发送应答
	
	