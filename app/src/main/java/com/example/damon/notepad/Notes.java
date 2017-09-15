package com.example.damon.notepad;

/**
 * Created by damon on 17-9-6.
 * 每一个日记条目所包含的属性.
 *
 */

public class Notes {

    int logo = R.drawable.logo01;      //logo图标
    String title;                   //标题
    String date = null;             //日期
    String contentKey;              //SharedPreferences  取内容的key
    String content;                 //记事内容
}
