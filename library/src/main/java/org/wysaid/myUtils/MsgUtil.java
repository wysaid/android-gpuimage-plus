package org.wysaid.myUtils;

import android.content.Context;
import android.view.View;
import android.widget.Toast;

import java.lang.ref.WeakReference;

/**
 * Created by wysaid on 16/2/26.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */
public class MsgUtil {

    static WeakReference<Context> mContext;
    static Toast mToast;

    public static Toast getCurrentToast() {
        return mToast;
    }

    public static void setCurrentToast(Context context, Toast toast) {
        mContext = new WeakReference<Context>(context);
        mToast = toast;
    }

    public static void clear() {
        mContext = null;
        mToast = null;
    }

    public static void toastMsg(Context context, String msg) {
        toastMsg(context, msg, Toast.LENGTH_LONG);
    }

    public static void toastMsg(Context context, String msg, int duration) {
        if(mContext == null || mContext.get() != context) {
            if(context == null) {
                mContext = null;
                return;
            }

            mContext = new WeakReference<Context>(context);
            mToast = Toast.makeText(mContext.get(), "", duration);
            mToast.setDuration(duration);
        }

        if(mContext.get() != null && mToast != null) {
//            mToast.cancel();
            mToast.setText(msg);
            mToast.show();
        }
    }

    public static boolean isDisplaying() {
        return mToast != null && (mToast.getView() != null && mToast.getView().getWindowVisibility() == View.VISIBLE);
    }
}
