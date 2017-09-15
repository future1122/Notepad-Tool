package com.example.damon.notepad;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.text.TextPaint;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.List;

/**
 * Created by damon on 17-9-8.
 */

public class ShowNotesBaseAdapter extends BaseAdapter {
    private MainActivity context;
    private List<Notes> notes ;
    //用户名
    String loginName;
    public ShowNotesBaseAdapter(List<Notes> notes,
                                MainActivity context){
        this.loginName = context.loginName;
        this.context = context;
        this.notes = notes;
    }
    @Override
    public int getCount() {
        return notes.size();
    }

    @Override
    public Object getItem(int i) {
        return notes.get(i);
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @Override
    public View getView(final int i, View view, ViewGroup viewGroup) {
        ViewHolder viewHolder;
        if(view == null){
            viewHolder = new ViewHolder();
            view = View.inflate(context,R.layout.show_notes_item,null);
            viewHolder.icon = (ImageView) view.findViewById(R.id.notes_icon);
            viewHolder.content = (TextView) view.findViewById(R.id.notes_content);
            viewHolder.date = (TextView) view.findViewById(R.id.notes_date);
            viewHolder.delete = (ImageButton) view.findViewById(R.id.notes_delete);
            viewHolder.title = (TextView) view.findViewById(R.id.notes_title);
            view.setTag(viewHolder);
        }else{
            viewHolder = (ViewHolder) view.getTag();
        }
        final Notes note = (Notes) getItem(i);
        viewHolder.icon.setImageResource(note.logo);
        viewHolder.title.setText(formatTitle(note.title));
        viewHolder.date.setText(note.date);
        viewHolder.content.setText(formatContent(note.content));
        //删除一个记事条目.
        viewHolder.delete.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new AlertDialog.Builder(context)
                        .setTitle("删除")
                        .setMessage("确定要删除这个记录吗?")
                        .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                //删除一个记事项目.
                                context.deleteNote(note);
                                context.handler.sendEmptyMessage(context.UPDATE_LISTVIEW);
                            }
                        })
                        .setNegativeButton("取消",null)
                        .show();
            }
        });
        //设置标题粗体
        TextPaint tp = viewHolder.title.getPaint();
        tp.setFakeBoldText(true);

        return view;
    }

    class ViewHolder{
        ImageView icon;     //小图标
        TextView title;     //标题
        TextView date;      //日期
        TextView content;   //开头部分内容
        ImageButton delete;      //删除按钮
    }
    //标题只显示前八个字.
    private String formatTitle(String s){
        String result;
        if(s.length()<=8) result = s;
        else result = s.substring(0,8)+"...";
        return result;
    }
    //内容只显示前10个字
    private String formatContent(String s){
        String result;
        if(s.length()<=10) result = s;
        else {
            result = s.substring(0,10)+"...";
            String[] strings = result.split("\\n");
            result = strings[0]+"...";
        }

        return result;
    }
}
