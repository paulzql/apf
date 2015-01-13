/**
 * 文件名  Plugin.java
 * 包含类 
 * 文件描述 TODO
 * 创建日期 Jun 28, 2013 10:41:40 AM
 * 版本信息 V1.0  
 * 作者    paul-ubuntu
 * 版权声明 Copyright (c) 2007—2013 Wootion.Co.Ltd. All rights reserved.
 */
package org.paul.apf;

import java.util.HashMap;
import java.util.Map;

/**
 * <p>类名 
 * <p>描述 TODO
 * <p>作者 paul-ubuntu
 * <p>创建日期 Jun 28, 2013
 * <p>修改记录：
 * <p> 日期          版本      修改者      描述
 *
 */
public class Plugin {
	/**
	 * 版本号
	 */
	private int versionCode = 0;
	/**
	 * 版本名称
	 */
	private String versionName = "0";
	/**
	 * 插件名称
	 */
	private String name  = "";
	/**
	 * 插件包
	 */
	private String packageName = "";
	/**
	 * 插件描述
	 */
	private String description = "";
	/**
	 * 插件中的类映射
	 */
	private Map<String, Class<?>> classMap= new HashMap<String, Class<?>>();
	/**
	 * @return the versionCode
	 */
	public int getVersionCode() {
		return versionCode;
	}
	/**
	 * @param versionCode the versionCode to set
	 */
	public void setVersionCode(int versionCode) {
		this.versionCode = versionCode;
	}
	/**
	 * @return the versionName
	 */
	public String getVersionName() {
		return versionName;
	}
	/**
	 * @param versionName the versionName to set
	 */
	public void setVersionName(String versionName) {
		this.versionName = versionName;
	}
	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}
	/**
	 * @param name the name to set
	 */
	public void setName(String name) {
		this.name = name;
	}
	/**
	 * @return the packageName
	 */
	public String getPackageName() {
		return packageName;
	}
	/**
	 * @param packageName the packageName to set
	 */
	public void setPackageName(String packageName) {
		this.packageName = packageName;
	}
	/**
	 * @return the description
	 */
	public String getDescription() {
		return description;
	}
	/**
	 * @param description the description to set
	 */
	public void setDescription(String description) {
		this.description = description;
	}
	/**
	 * @return the classMap
	 */
	public Map<String, Class<?>> getClassMap() {
		return classMap;
	}
	/**
	 * @param classMap the classMap to set
	 */
	public void setClassMap(Map<String, Class<?>> classMap) {
		this.classMap = classMap;
	}
	
}
