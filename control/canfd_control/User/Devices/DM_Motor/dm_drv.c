#include "dm_drv.h"

#include "fdcan.h"
#include "EventRecorder.h"
#include "stdio.h"

extern OpenArm_t arm;

float Hex_To_Float(uint32_t *Byte,int num)
{
	return *((float*)Byte);
}

uint32_t FloatTohex(float HEX)
{
	return *( uint32_t *)&HEX;
}

int float_to_uint(float x_float, float x_min, float x_max, int bits)
{
	/* Converts a float to an unsigned int, given range and number of bits */
	float span = x_max - x_min;
	float offset = x_min;
	return (int) ((x_float-offset)*((float)((1<<bits)-1))/span);
}

float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
	/* converts unsigned int to float, given range and number of bits */
	float span = x_max - x_min;
	float offset = x_min;
	return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}

void joint_motor_init(Joint_Motor_t *motor,uint16_t id, uint16_t master_id, uint16_t type)
{
	motor->slave_id=id;
	motor->master_id=master_id;
	motor->type = type;
	switch(motor->type)
		{
			case DM4310:
				motor->tmp.PMAX = P_MAX_4310;
				motor->tmp.VMAX = V_MAX_4310;
				motor->tmp.TMAX = T_MAX_4310;
				break;
			case DM4340:
				motor->tmp.PMAX = P_MAX_4340;
				motor->tmp.VMAX = V_MAX_4340;
				motor->tmp.TMAX = T_MAX_4340;
				break;
			case DM8009:
				motor->tmp.PMAX = P_MAX_8009;
				motor->tmp.VMAX = V_MAX_8009;
				motor->tmp.TMAX = T_MAX_8009;
				break;
			case DM3507:
				motor->tmp.PMAX = P_MAX_3507;
				motor->tmp.VMAX = V_MAX_3507;
				motor->tmp.TMAX = T_MAX_3507;
				break;
			default: 
				break;
		}
	
}

// feedback message callback function
void dm_fbdata(Joint_Motor_t *motor, uint8_t *data,uint32_t data_len)
{ 
	if (motor->slave_id == 0 || motor->slave_id > NUM_MOTORS) return;
	if(data_len!=FDCAN_DLC_BYTES_8) return;
	
	if(data[2] == 0x33 || data[2] == 0x55)
	{
		float_type_u y;
		
		uint8_t rid_value = data[3];
		y.b_val[0] = data[4];
		y.b_val[1] = data[5];
		y.b_val[2] = data[6];
		y.b_val[3] = data[7];
		
		switch (rid_value) 
		{
			case RID_UV_VALUE: motor->tmp.UV_Value = y.f_val; break;
			case RID_KT_VALUE: motor->tmp.KT_Value = y.f_val; break;
			case RID_OT_VALUE: motor->tmp.OT_Value = y.f_val; break;
			case RID_OC_VALUE: motor->tmp.OC_Value = y.f_val; break;
			case RID_ACC:      motor->tmp.ACC      = y.f_val; break;
			case RID_DEC:      motor->tmp.DEC      = y.f_val; break;
			case RID_MAX_SPD:  motor->tmp.MAX_SPD  = y.f_val; break;
			case RID_MST_ID:   motor->tmp.MST_ID   = y.u_val; break;
			case RID_ESC_ID:   motor->tmp.ESC_ID   = y.u_val; break;
			case RID_TIMEOUT:  motor->tmp.TIMEOUT  = y.u_val; break;
			case RID_CMODE:    motor->tmp.cmode    = y.u_val; break;
			case RID_DAMP:     motor->tmp.Damp     = y.f_val; break;
			case RID_INERTIA:  motor->tmp.Inertia  = y.f_val; break;
			case RID_HW_VER:   motor->tmp.hw_ver   = y.u_val; break;
			case RID_SW_VER:   motor->tmp.sw_ver   = y.u_val; break;
			case RID_SN:       motor->tmp.SN       = y.u_val; break;
			case RID_NPP:      motor->tmp.NPP      = y.u_val; break;
			case RID_RS:       motor->tmp.Rs       = y.f_val; break;
			case RID_LS:       motor->tmp.Ls       = y.f_val; break;
			case RID_FLUX:     motor->tmp.Flux     = y.f_val; break;
			case RID_GR:       motor->tmp.Gr       = y.f_val; break;
			case RID_PMAX:     motor->tmp.PMAX     = y.f_val; break;
			case RID_VMAX:     motor->tmp.VMAX     = y.f_val; break;
			case RID_TMAX:     motor->tmp.TMAX     = y.f_val; break;
			case RID_I_BW:     motor->tmp.I_BW     = y.f_val; break;
			case RID_KP_ASR:   motor->tmp.KP_ASR   = y.f_val; break;
			case RID_KI_ASR:   motor->tmp.KI_ASR   = y.f_val; break;
			case RID_KP_APR:   motor->tmp.KP_APR   = y.f_val; break;
			case RID_KI_APR:   motor->tmp.KI_APR   = y.f_val; break;
			case RID_OV_VALUE: motor->tmp.OV_Value = y.f_val; break;
			case RID_GREF:     motor->tmp.GREF     = y.f_val; break;
			case RID_DETA:     motor->tmp.Deta     = y.f_val; break;
			case RID_V_BW:     motor->tmp.V_BW     = y.f_val; break;
			case RID_IQ_CL:    motor->tmp.IQ_cl    = y.f_val; break;
			case RID_VL_CL:    motor->tmp.VL_cl    = y.f_val; break;
			case RID_CAN_BR:   motor->tmp.can_br   = y.u_val; break;
			case RID_SUB_VER:  motor->tmp.sub_ver  = y.u_val; break;
			case RID_U_OFF:    motor->tmp.u_off    = y.f_val; break;
			case RID_V_OFF:    motor->tmp.v_off    = y.f_val; break;
			case RID_K1:       motor->tmp.k1       = y.f_val; break;
			case RID_K2:       motor->tmp.k2       = y.f_val; break;
			case RID_M_OFF:    motor->tmp.m_off    = y.f_val; break;
			case RID_DIR:      motor->tmp.dir      = y.f_val; break;
			case RID_P_M:      motor->tmp.p_m      = y.f_val; break;
			case RID_X_OUT:    motor->tmp.x_out    = y.f_val; break;
		}
	}
	else
	{
		motor->slave_id = (data[0])&0x0F;
	  motor->state = (data[0])>>4;
	  motor->p_int=(data[1]<<8)|data[2];
	  motor->v_int=(data[3]<<4)|(data[4]>>4);
	  motor->t_int=((data[4]&0xF)<<8)|data[5];
		motor->pos = uint_to_float(motor->p_int, -motor->tmp.PMAX, motor->tmp.PMAX, 16); // 
		motor->vel = uint_to_float(motor->v_int, -motor->tmp.VMAX, motor->tmp.VMAX, 12); // 
		motor->tor = uint_to_float(motor->t_int, -motor->tmp.TMAX, motor->tmp.TMAX, 12);  // 
	  motor->Tmos = (float)(data[6]);
	  motor->Tcoil = (float)(data[7]);
	}
}

// sends command to motor to retrieve RID value
void read_motor_data(hcan_t* hcan, uint16_t id, uint8_t rid)
{
	uint8_t can_id_l = id & 0xFF;
	uint8_t can_id_h = (id >> 4) & 0x07;
	
	uint8_t data[4] = {can_id_l, can_id_h, 0x33, rid};
	canx_send_data(hcan, 0x7FF, data, 4);
}

// sends command to motor to retrieve control data
void read_ctrl_fbdata(hcan_t* hcan, uint16_t id)
{
	uint8_t can_id_l = id & 0xFF;
	uint8_t can_id_h = (id >> 4) & 0x07;
	
	uint8_t data[4] = {can_id_l, can_id_h, 0xCC, 0x00};
	canx_send_data(hcan, 0x7FF, data, 4);
}

//temporarily changes motor data
void change_motor_data(hcan_t* hcan, uint16_t id, uint8_t rid, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
	uint8_t can_id_l = id & 0xFF;
  uint8_t can_id_h = (id >> 8) & 0x07;
	
	uint8_t data[8] = {can_id_l, can_id_h, 0x55, rid, d0, d1, d2, d3};
	canx_send_data(hcan, 0x7FF, data, 8);
}

//permanently changes motor data
void write_motor_data(hcan_t* hcan, uint16_t id)
{
	uint8_t can_id_l = id & 0xFF;
  uint8_t can_id_h = (id >> 8) & 0x07;
	
	uint8_t data[4] = {can_id_l, can_id_h, 0xAA, 0x00};
	canx_send_data(hcan, 0x7FF, data, 4);
}

/**
************************************************************************
* @brief:      	enable_motor_mode: enables motor
* @param[in]:   hcan: handler for CANFD interface
* @param[in]:   motor_id: slave id of motor to disable
* @param[in]:   mode_id: the mode id defined in dm_drv.h
* @retval:     	void
************************************************************************
**/
void enable_motor_mode(hcan_t* hcan, uint16_t motor_id, uint16_t mode_id)
{
	uint8_t data[8];
	uint16_t id = motor_id + mode_id;
	
	data[0] = 0xFF;
	data[1] = 0xFF;
	data[2] = 0xFF;
	data[3] = 0xFF;
	data[4] = 0xFF;
	data[5] = 0xFF;
	data[6] = 0xFF;
	data[7] = 0xFC;
	
	canx_send_data(hcan, id, data, 8);
}
/**
************************************************************************
* @brief:      	disable_motor_mode: disables motor
* @param[in]:   hcan: handler for CANFD interface
* @param[in]:   motor_id: slave id of motor to disable
* @param[in]:   mode_id: the mode id defined in dm_drv.h
* @retval:     	void
************************************************************************
**/
void disable_motor_mode(hcan_t* hcan, uint16_t motor_id, uint16_t mode_id)
{
	uint8_t data[8];
	uint16_t id = motor_id + mode_id;
	
	data[0] = 0xFF;
	data[1] = 0xFF;
	data[2] = 0xFF;
	data[3] = 0xFF;
	data[4] = 0xFF;
	data[5] = 0xFF;
	data[6] = 0xFF;
	data[7] = 0xFD;
	
	canx_send_data(hcan, id, data, 8);
}
/**
************************************************************************
* @brief:      	set_zero_position: sets current position of motors as zero position
* @param[in]:   hcan: handler for CANFD interface
* @param[in]:   motor_id: slave id of motor to disable
* @param[in]:   mode_id: the mode id defined in dm_drv.h
* @retval:     	void
************************************************************************
**/
void set_zero_position(hcan_t* hcan, uint16_t motor_id){
	uint8_t data[8];
	uint16_t id = motor_id;
	
	data[0] = 0xFF;
	data[1] = 0xFF;
	data[2] = 0xFF;
	data[3] = 0xFF;
	data[4] = 0xFF;
	data[5] = 0xFF;
	data[6] = 0xFF;
	data[7] = 0xFE;
	
	canx_send_data(hcan, id, data, 8);
}


/**
************************************************************************
* @brief:      	mit_ctrl: moves one motor using MIT control based on given parameters
* @param[in]:   hcan:	handler for FDCAN bus
* @param[in]:   motor_id:	slave_id of motor in CAN bus
* @param[in]:   pos: desired position
* @param[in]:   vel: desired velocity
* @param[in]:   kp: position proportional gain
* @param[in]:   kd: position differential gain
* @param[in]:   torq: feedforward torque
* @retval:     	void
************************************************************************
**/
void mit_ctrl(hcan_t* hcan, uint16_t motor_id, float pos, float vel,float kp, float kd, float torq)
{
	Joint_Motor_t motor = arm.motors[motor_id-1];
	uint8_t data[8];
	uint16_t pos_tmp,vel_tmp,kp_tmp,kd_tmp,tor_tmp;
	uint16_t id = motor_id + MIT_MODE;
	
	pos_tmp = float_to_uint(pos, -motor.tmp.PMAX, motor.tmp.PMAX, 16); // 
	vel_tmp = float_to_uint(vel, -motor.tmp.VMAX, motor.tmp.VMAX, 12); // 
	tor_tmp = float_to_uint(torq, -motor.tmp.TMAX, motor.tmp.TMAX, 12);  // 
	kp_tmp  = float_to_uint(kp, KP_MIN, KP_MAX, 12);
	kd_tmp  = float_to_uint(kd, KD_MIN, KD_MAX, 12);
	
	data[0] = (pos_tmp >> 8);
	data[1] = pos_tmp;
	data[2] = (vel_tmp >> 4);
	data[3] = ((vel_tmp&0xF)<<4)|(kp_tmp>>8);
	data[4] = kp_tmp;
	data[5] = (kd_tmp >> 4);
	data[6] = ((kd_tmp&0xF)<<4)|(tor_tmp>>8);
	data[7] = tor_tmp;
	
	canx_send_data(hcan, id, data, 8);
}
/**
************************************************************************
* @brief:      	pos_speed_ctrl: position cascade mode that uses three-loop series control
* @param[in]:   hcan: handler for CANFD bus
* @param[in]:   motor_id:	slave id of motor in CAN bus
* @param[in]:   vel: desired velocity
* @retval:     	void
************************************************************************
**/
void pos_speed_ctrl(hcan_t* hcan, uint16_t motor_id, float pos, float vel)
{
	uint16_t id;
	uint8_t *pbuf, *vbuf;
	uint8_t data[8];
	
	id = motor_id + POS_MODE;
	pbuf=(uint8_t*)&pos;
	vbuf=(uint8_t*)&vel;
	
	data[0] = *pbuf;
	data[1] = *(pbuf+1);
	data[2] = *(pbuf+2);
	data[3] = *(pbuf+3);

	data[4] = *vbuf;
	data[5] = *(vbuf+1);
	data[6] = *(vbuf+2);
	data[7] = *(vbuf+3);
	
	canx_send_data(hcan, id, data, 8);
}
/**
************************************************************************
* @brief:      	speed_ctrl: runs motor at set speed
* @param[in]:   hcan: handler for CANFD bus
* @param[in]:   motor_id: slave id of motor in CAN bus
* @param[in]:   vel: desired velocity
* @retval:     	void
************************************************************************
**/
void speed_ctrl(hcan_t* hcan,uint16_t motor_id, float vel)
{
	uint16_t id;
	uint8_t *vbuf;
	uint8_t data[4];
	
	id = motor_id + SPEED_MODE;
	vbuf=(uint8_t*)&vel;
	
	data[0] = *vbuf;
	data[1] = *(vbuf+1);
	data[2] = *(vbuf+2);
	data[3] = *(vbuf+3);
	
	canx_send_data(hcan, id, data, 4);
}

/**
************************************************************************
* @brief:      	openarm_init: initializes OpenArm motors with provided parameters
* @param[in]:   arm:	pointer to OpenArm struct
* @param[in]:   id[]:	array of slave ids 
* @param[in]:   motor_type[]: array of motor types, refer to MotorType_t
* @param[in]:   mode[]: array of motor control modes
* @retval:     	void
************************************************************************
**/
void openarm_init(OpenArm_t *arm, int id[], int master_id[], int mode, int motor_type[]){
	arm->mode = mode;
	for(int i = 0; i < NUM_MOTORS; i++){
		joint_motor_init(&arm->motors[i], id[i], master_id[i], motor_type[i]);
	}
}

void openarm_enable(OpenArm_t *arm,  hcan_t *hcan){
	for(int i = 0; i < NUM_MOTORS; i++){
		enable_motor_mode(hcan, arm->motors[i].slave_id, arm->mode);
	}
}

void openarm_disable(OpenArm_t *arm,  hcan_t *hcan){
	for(int i = 0; i < NUM_MOTORS; i++){
		disable_motor_mode(hcan, arm->motors[i].slave_id, arm->mode);
	}
}

/**
************************************************************************
* @brief:      	move_mit: moves OpenArm according to MIT control params
* @param[in]:   arm:	pointer to OpenArm struct
* @param[in]:   hcan:	handler for CANFD interface
* @param[in]:   position[]:	array of desired position values
* @param[in]:   position[]:	array of desired velocity values
* @param[in]:   kp[]: array of position proportional gain
* @param[in]:   kd[]: array of position differential gain
* @param[in]:   tau[]: array of torque
* @retval:     	void
************************************************************************
**/
void move_mit_all(OpenArm_t *arm, hcan_t *hcan, float position[], float velocity[], float kp[], float kd[], float torque[]){
	for(int i = 0; i < NUM_MOTORS; i++){
		mit_ctrl(hcan, arm->motors[i].slave_id, position[i], velocity[i], kp[i], kd[i], torque[i]);
		while (HAL_FDCAN_GetTxFifoFreeLevel(hcan) == 0) {}
	}
}


