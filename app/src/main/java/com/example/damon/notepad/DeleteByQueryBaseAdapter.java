package com.example.damon.notepad;

import android.os.Handler;
import android.text.TextPaint;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.List;
import java.util.Map;

/**
 * Created by damon on 17-9-11.
 */

public class DeleteByQueryBaseAdapter extends BaseAdapter {
    MainActivity context;
    List<Notes> notes;
    String loginName;
    //判断是否选中的map
    Map<String,Boolean> checkedMap;
    //刷新界面的handler
    Handler handler;

    public DeleteByQueryBaseAdapter(MainActivity activity,List<Notes> notes){
        context = activity;
        this.handler = context.handler;
        this.checkedMap = context.checkedMap;
        this.loginName = context.loginName;
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
    public View getView(int i, View view, ViewGroup viewGroup) {
        final Integer id = i;
            final DeleteByQueryBaseAdapter.ViewHolder viewHolder;
            if(view == null){
                viewHolder = new DeleteByQueryBaseAdapter.ViewHolder();

                    view = View.inflate(context,R.layout.delete_by_query_item,null);
                    viewHolder.icon = (ImageView) view.findViewById(R.id.notes_icon);
                    viewHolder.content = (TextView) view.findViewById(R.id.notes_content);
                    viewHolder.date = (TextView) view.findViewById(R.id.notes_date);
                    viewHolder.choose = (CheckBox) view.findViewById(R.id.select);
                    viewHolder.title = (TextView) view.findViewById(R.id.notes_title);

                view.setTag(viewHolder);
            }else{
                viewHolder = (DeleteByQueryBaseAdapter.ViewHolder) view.getTag();
            }

                Notes note = (Notes) getItem(i);
                viewHolder.icon.setImageResource(note.logo);
                viewHolder.title.setText(formatTitle(note.title));
                viewHolder.date.setText(note.date);
                viewHolder.content.setText(formatContent(note.content));
                if(checkedMap.containsKey(notes.get(i).contentKey)) viewHolder.choose.setChecked(true);
                else viewHolder.choose.setChecked(false);
                viewHolder.choose.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        if(viewHolder.choose.isChecked()){
                            checkedMap.put(notes.get(id).contentKey,true);
                        }
                        else {
                            checkedMap.remove(notes.get(id).contentKey);

                        }
                        handler.sendEmptyMessage(context.UPDATE_DELETE_VIEW);
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
        CheckBox choose;    //选择框
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
