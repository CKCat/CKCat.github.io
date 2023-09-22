import frida, sys

def on_message(message, data):
    if message['type'] == 'send':
        print("[*] {0}".format(message['payload']))
    else:
        print(message)

jscode = """
Java.perform(function () {
    // 获取需要 hook 方法的类
    var MainActivity = Java.use('com.example.seccon2015.rock_paper_scissors.MainActivity');
    
    var MainActivity$1 = Java.use('com.example.seccon2015.rock_paper_scissors.MainActivity$1');

    var run = MainActivity$1.run;
    // 找到需要 hook 的方法
    var onClick = MainActivity.onClick;
    onClick.implementation = function (v) {
      // 与 Frida 进行通信
      send('onClick');
  
      // 调用原始函数
      onClick.call(this, v);
  
      // 原始函数调用完成后，修改相应的变量
      this.m.value = 0;
      this.n.value = 1;
      this.cnt.value = 999;

    };

    run.implementation = function () {
        // 与 Frida 进行通信
        send('run');
    
        run.call(this)
        
        // 输出 cnt 的值
        console.log('Done:' + JSON.stringify(this.this$0.value.cnt.value));
      };
  });
"""

process = frida.get_usb_device().attach('com.example.seccon2015.rock_paper_scissors')
script = process.create_script(jscode)
script.on('message', on_message)
print('[*] Running CTF')
script.load()
sys.stdin.read()