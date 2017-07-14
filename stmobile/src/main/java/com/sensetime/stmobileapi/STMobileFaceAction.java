package com.sensetime.stmobileapi;

import com.sensetime.stmobileapi.STMobileApiBridge.st_mobile_face_action_t;

/**
 * Created by zhym on 2016/8/23.
 */

public class STMobileFaceAction extends st_mobile_face_action_t {
    public STMobileFaceAction() {
    }

    public STMobileFaceAction(st_mobile_face_action_t origin) {
        face.rect = origin.face.rect;
        face.score = origin.face.score;
        face.yaw = origin.face.yaw;
        face.pitch = origin.face.pitch;
        face.roll = origin.face.roll;
        face.eye_dist = origin.face.eye_dist;
        face.ID = origin.face.ID;

        for(int i=0; i<face.points_array.length; i++) {
            face.points_array[i] = origin.face.points_array[i];
        }

        face_action = origin.face_action;
    }

    /**
     * Get face action
     * @return STMobile106
     * */
    public STMobile106 getFace() {
        STMobile106 s = new STMobile106();
        s.rect = face.rect;
        s.score = face.score;
        s.points_array = face.points_array;
        s.yaw = face.yaw;
        s.pitch = face.pitch;
        s.roll = face.roll;
        s.eye_dist = face.eye_dist;
        s.ID = face.ID;
        return s;
    }

    @Override
    public String toString() {
        return "STMobileFaceAction("+getFace()+", "+face_action+")";
    }
}
