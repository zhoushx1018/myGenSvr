

@brief myGenSvr, linux c\c++ server framework




 ���õ� one event loop per thread �� server���
���̶߳�ȡ�ͻ�������� server���
	
main
	���߳�
	������accept�������� socket���ӣ�ͨ��fd��ϣ���ض�SubReactor���󣬲��������ƽ�����SubReactor����
	

SubReactor
	ʹ�� Reactorģʽ
	ÿ��SubReactor����ֻ�� һ�� event loop ��һ�� thread

	�ڲ�ʹ��epoll ��Ϊͳһ��io�¼�����ʱ�¼��Ĵ�������
	�ⲿ�¼��������ӵ����������ͻ������ݵ�������ͨ���ܵ�ע�ᵽepoll���Ӷ���֤epoll�¼���ͳһ��

	����
  		�����ͻ������ݶ��У�
  		ͨ���ܵ����¼��������ö��еĶ��¼�

	�¼���
  		�ͻ��˶��¼�
  		�ܵ����¼�:�ͻ��������ӵĵ���
  		�ܵ����¼��������ͻ������ݵĵ���
  		�ڲ�����( ��epoll��ʱ��
  
SvrSender
	���������ڲ��д����Ͷ��У�
	�������ݵ�����ˣ��յ�Ӧ��󣬹�ϣ���ض� SubReactor���󣬷���Ӧ��
	
	