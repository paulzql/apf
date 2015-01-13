/**
 * 文件名  ClassManager.java
 * 包含类 
 * 文件描述 Class管理类文件
 * 创建日期 Jun 25, 2013 5:19:36 PM
 * 版本信息 V1.0  
 * 作者    paul-ubuntu
 * 版权声明 Copyright (c) 2007—2013 Wootion.Co.Ltd. All rights reserved.
 */
package org.paul.apf;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import android.util.Log;

/**
 * <p>类名 ClassManager
 * <p>描述 Class管理
 * <p>作者 paul-ubuntu
 * <p>创建日期 Jun 25, 2013
 * <p>修改记录：
 * <p> 日期          版本      修改者      描述
 *
 */
public class ClassManager {
	/**
	 * 注册的类列表（最新注册在前）
	 */
	private List<Class<?>> classList = new ArrayList<Class<?>>();
	/**
	 * 注册的类映射（类ID，类）
	 */
	private Map<String, Class<?>> classMap = new HashMap<String, Class<?>>();
	/**
	 * 接口与类的映射缓存
	 */
	private Map<Class<?>, Class<?>> cache = new HashMap<Class<?>, Class<?>>();
	/**
	 * 单实例对象
	 */
	private static ClassManager instance = null;
	
	/**
	 * 获取单实例对象
	 * 
	 * @return 单实例对象
	 * @throws
	 */
	public static ClassManager getInstance() {
		if (null == instance) {
			instance = new ClassManager();
		}
		
		return instance;
	}
	
	/**
	 * 注册类
	 * 跟据提供的类ID与类注册到其中，若类ID已被注册，则将替换为新的。
	 * @param clsid 类ID
	 * @param cls 类
	 * @throws
	 */
	public void registerClass(String clsid, Class<?> cls) {
		synchronized (this) {
			Class<?> tmpCls = classMap.get(clsid);
			if (null != tmpCls) {
				removeCache(tmpCls);
				// 从列表中删除
				classList.remove(cls);
			}
			classMap.put(clsid, cls);
			// 添加到列表最前面
			classList.add(0, cls);
		}
	}
	
	/**
	 * 移除注册类
	 * 
	 * @param clsid 类ID
	 * @throws
	 */
	public void unregisterClass(String clsid) {
		synchronized (this) {
			Class<?> tmpCls = classMap.get(clsid);
			if (null != tmpCls) {
				removeCache(tmpCls);
				classList.remove(tmpCls);
				classMap.remove(clsid);
			}
		}
	}
	
	/**
	 * 创建一个对象
	 * 跟据提供的需要实例化的类ID创建一个对象（若该类ID未被注册，则返回null)
	 * @param clsid 类ID
	 * @return 创建的对象,未创建成功返回null
	 * @throws
	 */
	public Object newInstance(String clsid) {
		Class<?> cls = null;
		Object   obj = null;
		synchronized(this) {
			cls = classMap.get(clsid);
		}
		
		if (null != cls) {
			try {
				obj = cls.newInstance();
			} catch (InstantiationException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			}
		}
		
		return obj;
	}
	
	/**
	 * 创建一个对象
	 * 跟据提供的接口类和需要实例化的类ID创建一个对象（若该类ID未被注册或注册的类未实现此接口，则返回null)
	 * @param icls 接口类
	 * @param clsid 类ID
	 * @return 创建的对象,未创建成功返回null
	 * @throws
	 */
	@SuppressWarnings("unchecked")
	public <T> T newInstance(Class<T> icls, String clsid) {
		T obj = null;
		Class<?> cls = null;
		
		if (null != icls) {
			if (null != clsid) {
				synchronized(this) {
					cls = classMap.get(clsid);
				}
				if (null != cls && icls.isAssignableFrom(cls)) {
					try {
						obj = (T)cls.newInstance();
					} catch (InstantiationException e) {
						e.printStackTrace();
					} catch (IllegalAccessException e) {
						e.printStackTrace();
					}
				} 
			}
		}
		
		return obj;		
	}
	
	/**
	 * 创建一个对象
	 * 跟据提供的接口类创建一个对象（若未找到实现该接口的类，则返回null)
	 * @param icls 接口类
	 * @return 创建的对象,未创建成功返回null
	 * @throws
	 */
	@SuppressWarnings("unchecked")
	public <T> T newInstance(Class<T> icls) {
		T obj = null;
		
		if (null != icls) {
			synchronized(this) {
				Class<?> cls = cache.get(icls);
				
				if (null == cls) {
					for (Class<?> tmpCls : classList) {
						if (null != tmpCls && icls.isAssignableFrom(tmpCls)) {
							try {
								obj = (T)tmpCls.newInstance();
							} catch (InstantiationException e) {
								e.printStackTrace();
							} catch (IllegalAccessException e) {
								e.printStackTrace();
							}
							
							cache.put(icls, tmpCls);
						} 						
					}
				}
			}
		}
		
		return obj;
	}
	
	/**
	 * 获取所有注册过的类集合
	 * 
	 * @return 注册的类集合
	 * @throws
	 */
	public Collection<Class<?>> getClasses() {
		return classMap.values();
	}
	
	/**
	 * 删除缓存
	 * 
	 * @param cls 要删除的相关类ID
	 * @throws
	 */
	private void removeCache(Class<?> cls) {
		// 删除缓存
		if (cache.containsValue(cls)) {
			List<Class<?>> removeList = new ArrayList<Class<?>>();
			Set<Entry<Class<?>, Class<?>>> set = cache.entrySet();
			Iterator<Entry<Class<?>, Class<?>>> it = set.iterator();
			
			while (it.hasNext()) {
				Entry<Class<?>, Class<?>> entry = it.next();
				if (entry.getValue().equals(cls)) {
					removeList.add(entry.getKey());
				}
			}
			
			for ( Class<?> rmCls : removeList ) {
				cache.remove(rmCls);
			}
		}
	}
	
}
