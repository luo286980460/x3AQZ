编译环境
	Qt5.15.2 MinGW64
	
注意:
    linux下串口需要写完整路径
	6e3p动态库用的是海涛最后发的版本
	
配置文件说明(cfg.json):
	{
		"httpServer":{			// 控屏服务属性
			"port":23332		// 控屏服务端口
		},
		"screen":{				// 屏幕属性
			"portName":"COM6"	// 控屏的端口号,linux端要写完整端口路径
		}
	}