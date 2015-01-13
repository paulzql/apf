/**
 * 文件名  PluginManager.java
 * 包含类 
 * 文件描述 TODO
 * 创建日期 Jun 27, 2013 11:29:42 AM
 * 版本信息 V1.0  
 * 作者    paul-ubuntu
 * 版权声明 Copyright (c) 2007—2013 Wootion.Co.Ltd. All rights reserved.
 */
package org.paul.apf;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.security.Provider;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.regex.Pattern;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;

import dalvik.system.DexClassLoader;
import dalvik.system.DexFile;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ProviderInfo;
import android.content.res.Resources;
import android.os.Environment;
import android.util.Log;

/**
 * <p>类名  PluginManager
 * <p>描述 插件也是一个Android工程生成的APK文件，但生成APK后要手动修改其中的classes.dex文件：
 * <p>    1. 将需要导出的类导出为Jar文件（千万不要导出在宿主程序中使用的接口）
 * <p>    2. 编译导出的jar为dex格式：$android-sdk/platform-tools/dx --dex --output=test-dex.jar test.jar
 * <p>    3. 解压出test-dex.jar中的classes.dex文件
 * <p>    4. 用classes.dex替换APK文件中的classes.dex
 * <p>作者 paul-ubuntu
 * <p>创建日期 Jun 27, 2013
 * <p>修改记录：
 * <p> 日期          版本      修改者      描述
 *
 */
public class PluginManager {
	/**
	 * 日志标签
	 */
	private final static String TAG = "PluginManager";
	/**
	 * 插件安装缓存目录后缀
	 */
	private final static String PLUGIN_CHACHE_SUFFIX = "-cache";
	/**
	 * 插件卸载标记后缀
	 */
	private final static String PLUGIN_REMOVE_FLAG_SUFFIX = "-remove";
	
	/**
	 * 上下文 
	 */
	private Context context = null;
	/**
	 * 插件目录
	 */
	private String pluginsPath = "";
	/**
	 * 插件列表
	 */
	private Map<String, Plugin> plugins = new HashMap<String, Plugin>();
	/**
	 * 单实例
	 */
	private static PluginManager instance = null;
	
	/**
	 * <p>构造函数</p>
	 * 
	 * @param context 上下文 
	 */
	public PluginManager(Context context) {
		this.context = context;
		// 初始化插件目录
		pluginsPath = context.getFilesDir() + File.separator + "apf";
		File file = new File(pluginsPath);
		if (!file.exists()) {
			file.mkdirs();
		}
		// 加载所有插件
		loadPlugins();
	}
	
	/**
	 * 获取插件基本信息
	 * 
	 * @param pluginFile 插件文件路径 
	 * @return 插件对象（包含基本信息), 获取不成功返回null
	 * @throws
	 */
	public Plugin getPluginBaseInfo(String pluginFile) {
		Plugin plugin = null;
		
		// 检查文件路径
		File file = new File(pluginFile);
		if (!file.exists() || !file.isFile()) {
			Log.w("PluginManager", "cannot parse file:"+pluginFile);
			return null;
		}
		
		PackageManager pm = context.getPackageManager();
		PackageInfo packageInfo = pm.getPackageArchiveInfo(pluginFile, PackageManager.GET_PROVIDERS);
		
		if (null != packageInfo) {
			plugin = new Plugin();
			plugin.setName(packageInfo.applicationInfo.name);
			plugin.setPackageName(packageInfo.applicationInfo.packageName);
			plugin.setVersionCode(packageInfo.versionCode);
			plugin.setVersionName(packageInfo.versionName);
		}
		
		return plugin;
	}
	
	/**
	 * 获取插件对象
	 * 
	 * @param pluginPackage 插件包名
	 * @return 插件对象
	 * @throws
	 */
	public final Plugin getPlugin(String pluginPackage) {
		synchronized(this) {
			return plugins.get(pluginPackage);
		}
	}
	
	/**
	 * 获取所有插件
	 * 
	 * @return 插件集合
	 * @throws
	 */
	public Collection<Plugin> plugins() {
		synchronized(this) {
			return plugins.values();
		}
	}
	
	/**
	 * 安装插件
	 * 
	 * @param pluginFile 插件文件路径
	 * @return 安装是否成功
	 * @throws
	 */
	public boolean install(String pluginFile) {
		Plugin plugin = getPluginBaseInfo(pluginFile);
		if (null == plugin) {
			return false;
		}

		String pluginPath = pluginsPath + File.separator + plugin.getPackageName();
		String cachePath = pluginPath + PLUGIN_CHACHE_SUFFIX;
		File targetFile = new File(pluginPath);
		File srcFile = new File(cachePath);
		
		synchronized(this) {
			// 检查版本
			Plugin installedPlugin = getPlugin(plugin.getPackageName());
			if (null != installedPlugin && 
					installedPlugin.getVersionCode() >= plugin.getVersionCode()) {
				return false;
			}
			
			if (!srcFile.exists()) {
				srcFile.mkdirs();
			}
			
			// 解压库文件到缓存目录
			if (!releaseZipToPath(pluginFile, cachePath, "^(lib).*")) {
				//删除临时文件
				deleteDir(srcFile);
				return false;
			}
			
			try {
				// 拷贝插件文件
				copyFile(pluginFile, cachePath + File.separator + plugin.getPackageName()+ ".apk");
			} catch (IOException e) {
				e.printStackTrace();
				//删除临时文件
				deleteDir(srcFile);
				return false;
			}
			
			// 卸载原来的版本
			if (null != installedPlugin) {
				plugins.remove(installedPlugin.getPackageName());
			}
			
			if (!targetFile.exists() || deleteDir(targetFile)) {
				srcFile.renameTo(targetFile);
			}
			
			File flagFile = new File(pluginPath + PLUGIN_REMOVE_FLAG_SUFFIX);
			if (flagFile.exists()) {
				flagFile.delete();
			}
			
			plugin = loadPlugin(targetFile.getAbsolutePath()+File.separator+targetFile.getName()+".apk");
		}
		return true;
	}
	
	/**
	 * 卸载插件
	 * 
	 * @param pluginPackage 插件包名
	 * @return 卸载是否成功
	 * @throws
	 */
	public boolean uninstall(String pluginPackage) {
		synchronized(this) {
			plugins.remove(pluginPackage);
			String pluginPath = pluginsPath + File.separator + pluginPackage;
			String cachePath = pluginPath + PLUGIN_CHACHE_SUFFIX;
			String removePath = pluginPath + PLUGIN_REMOVE_FLAG_SUFFIX;
			
			File pluginFile = new File(pluginPath);
			File cacheFile = new File(cachePath);
			
			// 删除插件目录
			deleteDir(pluginFile);
			deleteDir(cacheFile);
			// 创建删除标记
			if (pluginFile.exists()) {
				File removeFile = new File(removePath);
				try {
					removeFile.createNewFile();
				} catch (IOException e) {
					e.printStackTrace();
					return false;
				}
			}
		}
		
		return true;
	}
	
	/**
	 * 加载安装的插件
	 * 
	 * @throws
	 */
	private void loadPlugins() {
		File pluginsFile = new File(pluginsPath);

		for (File file : pluginsFile.listFiles()) {
			Plugin plugin = null;
			// 删除
			if (file.getName().endsWith(PLUGIN_REMOVE_FLAG_SUFFIX) && file.exists()) {
				deleteDir(new File(file.getAbsolutePath().substring(0, 
						file.getAbsolutePath().length()-PLUGIN_REMOVE_FLAG_SUFFIX.length())));
				deleteDir(new File(file.getAbsolutePath().substring(0, 
						file.getAbsolutePath().length()-PLUGIN_REMOVE_FLAG_SUFFIX.length())+PLUGIN_CHACHE_SUFFIX));
				deleteDir(file);
			// 更新
			} else if (file.getName().endsWith(PLUGIN_CHACHE_SUFFIX) && file.exists()) {
				String pluginPath = file.getAbsolutePath().substring(0, 
						file.getAbsolutePath().length()-PLUGIN_CHACHE_SUFFIX.length());
				deleteDir(new File(pluginPath));
				
				File pluginDir = new File(pluginPath);
				file.renameTo(pluginDir);
				
				plugin = loadPlugin(pluginDir.getAbsolutePath()+File.separator+pluginDir.getName()+".apk");
			} else if (file.exists()){
				plugin = loadPlugin(file.getAbsolutePath()+File.separator+file.getName()+".apk");
			}
			
			if (null != plugin) {
				plugins.put(plugin.getPackageName(), plugin);
				Log.i(TAG, "load plugin:"+plugin.getPackageName()+" ok");
			}
		}
	}

	/**
	 * 获取插件管理器单实例
	 * 
	 * @param context 上下文 (第一次需要传入值，以后可以不用传)
	 * @return 单实例对象
	 * @throws
	 */
	public static PluginManager getInstance(Context context) {
		if (null == instance) {
			instance = new PluginManager(context);
		}
		
		return instance;
	}
	
	/**
	 * 加载一个插件文件
	 * 
	 * @param pluginFile 插件文件
	 * @return 插件对象
	 * @throws
	 */
	private Plugin loadPlugin(String pluginFile) {
		Plugin plugin = null;
	
		plugin = getPluginBaseInfo(pluginFile);
		if (null == plugin) {
			return null;
		}
		
		File file = new File(pluginFile);
		PackageManager pm = context.getPackageManager();
		PackageInfo packageInfo = pm.getPackageArchiveInfo(pluginFile, PackageManager.GET_PROVIDERS);
		DexClassLoader classloader = new DexClassLoader(file.getAbsolutePath(), 
				file.getParent(), pluginFile + File.separator + "lib", context.getClassLoader());
		
		Map<String, Class<?>> pluginClassMap = new HashMap<String, Class<?>>();
		// 插件的类映射放在provider中
		ProviderInfo[] providers = packageInfo.providers;
		if (null != providers) {
			for (ProviderInfo provider : providers) {
				try {
					// 加载类
					Class<?> cls = classloader.loadClass(provider.name);
					if (provider.name.length() > 0) {
						pluginClassMap.put(provider.authority, cls);
					} else {
						pluginClassMap.put(provider.name, cls);
					}
				} catch (ClassNotFoundException e) {
					e.printStackTrace();
				}
			}
		}
		// 设置插件类信息
		plugin.setClassMap(pluginClassMap);
		// 注册类信息
		Iterator<Entry<String,Class<?>>> it = pluginClassMap.entrySet().iterator();
		while (it.hasNext()) {
			Entry<String,Class<?>> entry = it.next();
			ClassManager.getInstance().registerClass(entry.getKey(), entry.getValue());
		}
		return plugin;
	}
	
	/**
	 * 获取插件资源对象
	 * 
	 * @param packageName 插件包名
	 * @return
	 * @throws
	 */
	public Resources getPluginResource(String packageName) {
		return getAPKResource(pluginsPath+File.separator+packageName+File.separator+packageName+".apk");
	}
	
	/**
	 * 从APK文件中获取资源对象
	 * 
	 * @param apkPath apk文件路径
	 * @return
	 * @throws
	 */
	private Resources getAPKResource(String apkPath) {
		Resources res = null;
		try {
			//反射得到assetMagCls对象并实例化,无参  
			Class<?> assetMagCls = Class.forName("android.content.res.AssetManager");     
			Object assetMag = assetMagCls.newInstance();
			
			//从assetMagCls类得到addAssetPath方法  
			Class<?>[] typeArgs = new Class[1];  
			typeArgs[0] = String.class;  
			Method assetMag_addAssetPathMtd = assetMagCls.getDeclaredMethod("addAssetPath", typeArgs);  
			Object[] valueArgs = new Object[1];  
			valueArgs[0] = apkPath;  
			
			//执行assetMag_addAssetPathMtd方法  
			assetMag_addAssetPathMtd.invoke(assetMag, valueArgs);  
			
			//得到Resources对象并实例化,有参数  
			Resources mainRes = context.getResources();  
			typeArgs = new Class[3];  
			typeArgs[0] = assetMag.getClass();  
			typeArgs[1] = mainRes.getDisplayMetrics().getClass();  
			typeArgs[2] = mainRes.getConfiguration().getClass();  
			Constructor<Resources> resCt = Resources.class.getConstructor(typeArgs);  
			valueArgs = new Object[3];  
			valueArgs[0] = assetMag;  
			valueArgs[1] = mainRes.getDisplayMetrics();  
			valueArgs[2] = mainRes.getConfiguration();  
			
			//这个是重点
			//得到Resource对象后可以有很多用处
			res = (Resources) resCt.newInstance(valueArgs);
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		}
		
		return res;
	}
	
	/**
	 * 解压zip文件到指定目录下
	 *
     * @param sourceZip 
     * 		要解压的zip文件路径
     * @param outPath 
     * 		解压的目标目录
     * @param filter 
     * 		正则匹配
     * @return 解压是否成功
	 */
	private static boolean releaseZipToPath(String sourceZip, String outPath, String filter) {
		ZipFile zfile = null;
		Pattern filterPattern = null;
		boolean ret = true;
		
		try {
			zfile = new ZipFile(sourceZip);
		} catch (IOException e) {
			Log.e(TAG, "can't unzip file: "+sourceZip);
			e.printStackTrace();
			return false;
		}
		if (null != filter) {
			filterPattern = Pattern.compile(filter);
		}
		Enumeration<? extends ZipEntry> zList=zfile.entries();
		ZipEntry ze=null;
		byte[] buf=new byte[1024];
		// 遍历zip文件进行解压操作
		while (zList.hasMoreElements()) {
			// 从ZipFile中得到一个ZipEntry
			ze=(ZipEntry)zList.nextElement();
			if (ze.isDirectory()) {
				continue;
			}
			// 检查是否匹配
			if (null != filterPattern && !filterPattern.matcher(ze.getName()).matches()) {
				continue;
			}
			
			File pathFile = getRealFilePath(outPath, ze.getName());
			// 获取OutputStream
			OutputStream os = null;
			try {
				os = new BufferedOutputStream(new FileOutputStream(pathFile));
			} catch (FileNotFoundException e) {
				e.printStackTrace();
				Log.e(TAG, "create path:"+pathFile.getAbsolutePath()+" failed");
				ret = false;
				break;
			}
			// 以ZipEntry为参数得到一个InputStream，并写到OutputStream中
			try {
				int readLen=0;
				InputStream is = new BufferedInputStream(zfile.getInputStream(ze));
				
				while ((readLen=is.read(buf, 0, 1024))!=-1) {
					os.write(buf, 0, readLen);
				}
				is.close();
				os.close();
			} catch (IOException e) {
				e.printStackTrace();
				ret = false;
				break;
			}
		}
		// 关闭zip文件
		try {
			zfile.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return ret;
	}
	
	/**
	 * 给定根目录，返回一个相对路径所对应的实际文件名.
	 *
	 * @param baseDir
	 *            指定根目录
	 * @param absFileName
	 *            相对路径名，来自于ZipEntry中的name
	 * @return java.io.File 实际的文件
	 */
	private static File getRealFilePath(String baseDir, String absFileName) {
	        String[] dirs = absFileName.split("/");
	        File ret = new File(baseDir);

	        if (dirs.length > 1) {
	                for (int i = 0; i < dirs.length - 1; i++) {
	                        ret = new File(ret, dirs[i]);
	                }
	        }
	        
	        if (!ret.exists()) {
	                ret.mkdirs();
	        }
	        ret = new File(ret, dirs[dirs.length - 1]);
	        
	        return ret;
	}
	
    /**
     * 递归删除目录下的所有文件及子目录下所有文件
     * @param dir 将要删除的文件目录
     * @return boolean 删除是否成功
     */
    private static boolean deleteDir(File dir) {
        if (dir.isDirectory()) {
            String[] children = dir.list();
            //递归删除目录中的子目录下
            for (int i=0; i<children.length; i++) {
                boolean success = deleteDir(new File(dir, children[i]));
                if (!success) {
                    return false;
                }
            }
        }
        // 目录此时为空，可以删除
        return dir.delete();
    }
    
    /**
     * 复制文件
     * 
     * @param source 源文件
     * @param target 目标文件
     * @throws IOException
     */
    private static void copyFile(String source, String target) throws IOException {
        BufferedInputStream inBuff = null;
        BufferedOutputStream outBuff = null;
        try {
            // 新建文件输入流并对它进行缓冲
            inBuff = new BufferedInputStream(new FileInputStream(new File(source)));
            File targetFile = new File(target);
            if (!targetFile.exists()) {
            	targetFile.createNewFile();
            }
            // 新建文件输出流并对它进行缓冲
            outBuff = new BufferedOutputStream(new FileOutputStream(targetFile));

            // 缓冲数组
            byte[] b = new byte[1024 * 5];
            int len;
            while ((len = inBuff.read(b)) != -1) {
                outBuff.write(b, 0, len);
            }
            // 刷新此缓冲的输出流
            outBuff.flush();
        } finally {
            // 关闭流
            if (inBuff != null)
                inBuff.close();
            if (outBuff != null)
                outBuff.close();
        }
    }
}
