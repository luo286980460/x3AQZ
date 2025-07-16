编译环境
	Qt5.15.2 MinGW64
	
注意:
    linux下需要rdkafka动态库
	kafka主题可打开多个,打开以后可从接口往打开的主题传送数据
	
配置文件说明(cfg.json):
	{
		"httpServer":{						// 本地kafka创造者服务属性
			"port":23333					// 本地kafka创造者服务端口
		},
		"kafka":{							// kafka主机服务属性
			"ipPort":"58.240.67.138:18543",	// kafka主机服务ip端口
			"topics":[						// 创造者连接的主题
				"LS_TEST1",					// 主题1
				"LS_TEST2"					// 主题2
			]
		}
	}