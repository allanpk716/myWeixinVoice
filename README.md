myWeixinVoice
=============

微信语音识别，基于讯飞。


说明：
    
    本程序用于微信后台的语音识别（语义识别），当然微信也说提供了这种接口，不过是面向服务号，而且貌似没提到有语义识别。
    首先，你要有一个讯飞的appid。
    
    	//appid 请勿随意改动
	    ret = QISRInit("appid=");
	    
	  然后，本程序是写给php使用的，而本程序仅仅是做语义分析。
	  
	  PHP调用外部程序：
	  exec($convertVoiceFileAll, $outArray, $var);
	  
	  
	  php获取C++什么的返回参数，这里主要是以printf打印出来返回的，所以不要pritf太多多余的东西，后面也不好处理。
	  之后对于 $outArray 返回的数组进行返回值的判断。当然，返回值为了避免不必要的字符串问题，以URL的方式返回，
	  所以在php中需要对url做一次decode。
	  
	  
	  对于讯飞的使用，具体请参考讯飞官方文档。
	  
	  写的很随意，上传上来主要是为了多个地方想更新的时候方便一点。
	  

如何使用：

    讯飞识别需要用wav文件，而且是特定格式，详细就不说了，可以看代码，或者偷懒的话，用ffmpeg来转吧。
    
    ffmpeg -i D:123.m4a -ar 11025 -ac 1 -acodec pcm_s16le d:12345.wav

    一般来说PHP文件用 utf-8 的编码格式，那么，再传入参数之前，需要转码。
    
    用 base64 加密过的传参,而且在base64加密前，这个字符串要转换为GBK2312，也就需要在php中进行。
    
    myWeixinVoice.exe ZDpcVm9pY2VGaWxlXLe4vPpfMjAxMzExMDdfMTIyOTI2Lndhdg== 123
    
    第三个参数随便写，具体看代码。
    
    
语义文件：
    
    gm_continuous_mytv.abnf ，此文件的语法请看 讯飞 官方的文档。这个仅仅是示例，也许会更新。
