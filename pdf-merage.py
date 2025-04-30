# -*- coding: utf-8 -*-
'''
Created on Wed Apr 30 09:29:10 2025

@author: capre.0805
'''

'''
@これを使う人へ
以下をターミナル or コマンドプロンプトで入力してから使う
pip install pypdf
pip show pypdf
'''


from pypdf import PdfReader, PdfWriter

def parse_pages(pages_spec, total_pages):
    '''
    ページ指定を 0始まりのリストに変換
    None や "all" は全ページを意味する
    例: "1-3,5" → [0, 1, 2, 4]
    '''
    if pages_spec is None or str(pages_spec).lower() == 'all':
        return list(range(total_pages))  # 全ページ

    result = []
    if isinstance(pages_spec, str):
        parts = pages_spec.split(',')
        for part in parts:
            if '-' in part:
                start, end = map(int, part.split('-'))
                result.extend(range(start - 1, end))
            else:
                result.append(int(part) - 1)
    elif isinstance(pages_spec, list):
        result = [p - 1 for p in pages_spec]
    return result

def merge_selected_pages(pdf_pages_info, output_path):
    '''
    pdf_pages_info: List of tuples → (ファイル名, ページ指定)
        ページ指定は "1-3,5", [1, 3, 5], None, または "all"
    output_path: 出力先ファイル名
    '''
    writer = PdfWriter()

    for file_name, page_spec in pdf_pages_info:
        reader = PdfReader(file_name)
        total = len(reader.pages)
        page_nums = parse_pages(page_spec, total)
        for page_num in page_nums:
            if 0 <= page_num < total:
                writer.add_page(reader.pages[page_num])
            else:
                print(f"Warning: {file_name} に {page_num+1} ページは存在しません")

    with open(output_path, "wb") as f_out:
        writer.write(f_out)

# 使用例
pdf_pages = [
    ('0.pdf', "all"),    
    ('5.pdf', "all"),    
    ('6.pdf', "all"),  
    ('3.pdf', "all"),       
    ('2.pdf', "all"),  
    ('7.pdf', "all"),      
    ('1.pdf', "all"),       
    ('4.pdf', "all"),       
    ('9.pdf', "all"),       
    ('8.pdf', "all")        
]

merge_selected_pages(pdf_pages, 'output.pdf')
