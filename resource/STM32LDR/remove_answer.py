import re

# 读取文件内容
with open('./Core/Src/main.c', 'r', encoding='utf-8') as f:
    content = f.read()

# 定义要替换的正则表达式 - 选择所有答案
pattern = r' */{3,} PS: The answer is as follows:.*?/{3,} PS: End of question answer.'
# re.DOTALL表示.可以匹配换行符
new_content = re.sub(pattern, '', content, flags=re.DOTALL)

# 删除答案后，前后加在一起有3个换行，把三个换行替换成一个换行
pattern = r'\n{3}'
new_content = re.sub(pattern, '\n', new_content)

# 将替换后的内容写入文件
with open('./Core/Src/main_需要作答.txt', 'w', encoding='utf-8') as f:
    f.write(new_content)
