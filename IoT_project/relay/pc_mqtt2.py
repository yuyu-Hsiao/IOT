import tkinter as tk
from tkinter import scrolledtext
import paho.mqtt.client as mqtt

# 定義MQTT伺服器的位址及埠
MQTT_SERVER = ""  
MQTT_PORT = 1883  
SUB_MQTT_TOPIC = "myTopic/relay"  # 訂閱的主題
PUB_MQTT_TOPIC = "myTopic/relay" # 發布的主題

current_topic = PUB_MQTT_TOPIC

# 當連接到MQTT伺服器時呼叫此函式
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("成功連接到MQTT伺服器")
        client.subscribe(SUB_MQTT_TOPIC)
    else:
        print(f"連接失敗，返回代碼：{rc}")

# 當從MQTT伺服器收到訊息時呼叫此函式
def on_message(client, userdata, msg):
    message = f"收到訊息：{msg.topic} {msg.payload.decode()}\n"
    text_area.configure(state='normal')
    text_area.insert(tk.END, message)
    text_area.configure(state='disabled')

# 發布訊息
def publish_message():
    message = entry.get()
    client.publish(current_topic, message)
    entry.delete(0, tk.END)

# 更改主題
def change_topic():
    global current_topic
    new_topic = topic_entry.get()
    client.unsubscribe(current_topic)
    current_topic = new_topic
    client.subscribe(current_topic)
    topic_entry.delete(0, tk.END)
    print(f"訂閱已更改為：{current_topic}")

# 建立MQTT客戶端
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# 嘗試連接到MQTT伺服器並處理可能的錯誤
try:
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
except Exception as e:
    print(f"連接MQTT伺服器時發生錯誤: {e}")

# 啟動一個循環以處理網路流量和回呼
client.loop_start()

# 建立tkinter主視窗
root = tk.Tk()
root.title("MQTT 客戶端")

# 訂閱訊息顯示區域
text_area = scrolledtext.ScrolledText(root, wrap=tk.WORD, width=50, height=15, state='disabled')
text_area.grid(column=0, row=0, padx=10, pady=10, columnspan=2)

# 發布訊息輸入框
entry = tk.Entry(root, width=40)
entry.grid(column=0, row=1, padx=10, pady=10)

# 發布按鈕
publish_button = tk.Button(root, text="發布", command=publish_message)
publish_button.grid(column=1, row=1, padx=10, pady=10)

# 新主題輸入框
topic_entry = tk.Entry(root, width=40)
topic_entry.grid(column=0, row=2, padx=10, pady=10)

# 更改主題按鈕
change_topic_button = tk.Button(root, text="更改主題", command=change_topic)
change_topic_button.grid(column=1, row=2, padx=10, pady=10)

# 運行tkinter主迴圈
root.mainloop()

# 停止MQTT客戶端
client.loop_stop()
client.disconnect()
