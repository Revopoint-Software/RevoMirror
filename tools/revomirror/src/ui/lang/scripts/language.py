
import xlrd
import os
import optparse
import codecs

# pip uninstall xlrd
# pip install xlrd==1.2.0

# xml字符转义
def convertXmlString(inputText):
    if not isinstance(inputText, str):
        print(inputText,'这不是一个字符串，不进行处理')
        return
    #去掉回车
    inputText = inputText.replace("\r", "")

    inputText = inputText.replace("<", "&lt;")
    inputText = inputText.replace(">", "&gt;")
    inputText = inputText.replace("&", "&amp;")
    inputText = inputText.replace("'", "&apos;")
    inputText = inputText.replace("\"", "&quot;")
    return inputText

def createLange(base_path, path):
    print(os.path.abspath(base_path))
    path1 = os.path.split(os.path.abspath(base_path))
    path = os.path.abspath(path)
    book = xlrd.open_workbook(base_path)
    sheet1 = book.sheets()[0]
    nrows = sheet1.nrows
    print('表格总行数', nrows)
    ncols = sheet1.ncols
    print('表格总列数', ncols)

#默认值    
#第一行为表头，第二行开始为数据    
#读title,默认为：第一列为图片，第二列为key，第三列为中文
#key 列
    keyCol = 1
#主语言列（现指中文）
    mainLanageCol =2
    lanContent = ['']*ncols
    titles = sheet1.row_values(0)
    print('title: ', titles)
    langFileContent = '#ifndef LANGUAGE_H\n#define LANGUAGE_H\n\n'
    #文件行
    langRow = 1
    for j in range(0, ncols): 
        lanContent[j] = '<?xml version="1.0" encoding="utf-8"?>\n<!DOCTYPE TS>\n<TS version="2.1" language="{}">\n'.format(titles[j])
        lanContent[j] +='<context>\n\t<name>QObject</name>\n'
    for i in range(1, nrows):
        rowValue = sheet1.row_values(i)
        # 生成language.h文件
        if not rowValue[keyCol] =="":
            text = rowValue[mainLanageCol]
            #去掉回车
            text = text.replace("\r", "")
            #换行转义为c++标准字符串
            text = text.replace("\n", "\\n")
            langFileContent += '#define {}\t\tQObject::tr("{}")\n'.format(rowValue[keyCol], text)
            langRow += 1
            
            for j in range(0, ncols):
                lanContent[j] += '\t<message>\n'
                lanContent[j] += '\t\t<location filename="language.h" line="{}"/>\n'.format(langRow)
                lanContent[j] += '\t\t<source>{}</source>\n'.format(convertXmlString(rowValue[mainLanageCol]))
                lanContent[j] += '\t\t<translation>{}</translation>\n'.format(convertXmlString(rowValue[j]))
                lanContent[j] += '\t\t</message>\n'

    for j in range(0, ncols):    
        lanContent[j] += '</context>\n</TS>\n'    
        # print(lan[j])

    
    # print(langFileContent)
    
    #保存到数据文件中，并转换为qm，从第三列开始为语言
    for j in range(3, ncols):
        if not titles[j] =="":  
            fileName = path + "/language_{}.ts".format(titles[j])
            f = open(fileName, 'w', encoding='utf-8-sig')   
            f.write(lanContent[j])
            f.close()
            
            #生成qm
            os.system('lrelease {}'.format(fileName))
            os.remove(fileName)
            # print(lan[j])   
        
    langFileContent += '\n#endif //LANGUAGE_H\n'
    f = open(path1[0]+ '/language.h', 'w', encoding='utf-8-sig')   
    f.write(langFileContent)
    f.close()
    
def main():
    usage="Usage: %prog [options] langExcel outpath"
    parser=optparse.OptionParser(usage,version="%prog 1.0")
    options,args=parser.parse_args()    
    if len(args) != 2:  
        parser.error("incorrect number of arguments")      
    # print(args[0])
    createLange(args[0], args[1])
    
    # createLange("d:/scanla_pc.xls")
    
if __name__ == '__main__':
    main()    