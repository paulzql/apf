/**
 * 文件名  Interface.java
 * 包含类 
 * 文件描述 TODO
 * 创建日期 Jun 25, 2013 11:31:59 AM
 * 版本信息 V1.0  
 * 作者    paul-ubuntu
 * 版权声明 Copyright (c) 2007—2013 Wootion.Co.Ltd. All rights reserved.
 */
package org.paul.apf;


/**
 * <p>类名 
 * <p>描述 接口类
 * <p>作者 paul-ubuntu
 * <p>创建日期 Jun 25, 2013
 * <p>修改记录：
 * <p> 日期          版本      修改者      描述
 *
 */
public class Interface {
	/**
	 * 创建一个对象
	 * 跟据提供的接口类和需要实例化的类ID创建一个对象（若该类ID未被注册或注册的类未实现此接口，则返回null)
	 * @param icls 接口类
	 * @param clsid 类ID
	 * @return 创建的对象
	 * @throws
	 */	
	public static <T> T newInstance(Class<T> icls, String clsid) {
		return ClassManager.getInstance().newInstance(icls, clsid);
	}
	
	/**
	 * 创建一个对象
	 * 跟据提供的接口类创建一个对象（若未找到实现该接口的类，则返回null)
	 * @param icls 接口类
	 * @return 创建的对象
	 * @throws
	 */	
	public static <T> T newInstance(Class<T> icls) {
		return ClassManager.getInstance().newInstance(icls);
	}
	
	/**
	 * 创建一个对象
	 * 跟据提供的需要实例化的类ID创建一个对象（若该类ID未被注册，则返回null)
	 * @param clsid 类ID
	 * @return 创建的对象,未创建成功返回null
	 * @throws
	 */
	public Object newInstance(String clsid) {
		return ClassManager.getInstance().newInstance(clsid);
	}
}
