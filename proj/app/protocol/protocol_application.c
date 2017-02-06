/********************************************************
 * File	       :  protocol_application.h
 * Description :  Ӧ��Э��㣬������
 * Author      :  ��
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Update      :  2015-06-26
 * History     :

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol_typedef.h"
#include "protocol_base64.h"


#define ENCRYPT (1)
#define DECRYPT (0)

static PProtocol_Layer_ST Self = 0;

extern int protocol_tripledes(unsigned char *key,unsigned char *src,unsigned char *dest,int m);
extern unsigned short crc_ccitt(unsigned char *q, int len,unsigned short crc);


static unsigned char* protocol_Application_Section(unsigned char* data,unsigned short datalen,unsigned char** line);
static void Protocol_Application_ParamParse(PProtocol_Layer_ST layer,unsigned char* param,unsigned short datalen);
static void Protocol_Application_Parse(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen);


/********************************************************
 * function    :  Protocol_Application_Init
 * Description :  Э����ʼ������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_Init(PProtocol_Layer_ST layer)
{
    if (!layer->down)
    {
        return;
    }

    layer->down->init(layer->down);
}

/********************************************************
 * function    :  Protocol_Application_Pack
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_Pack(PProtocol_Layer_ST layer)
{
    if (layer->datalen)
    {
        layer->data[0] = '{';
        layer->data[layer->datalen] = '}';
        layer->datalen = strlen(layer->data);
    }
    if (!layer->down)
    {
        layer->send(layer);
        return;
    }

    layer->down->pack(layer->down);
}

/********************************************************
 * function    :  Protocol_Application_Unpack
 * Description :  �������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_Unpack(PProtocol_Layer_ST layer)
{
    unsigned char* pbuffer;
    unsigned short datalen;
    if (!layer->down)
    {
        pbuffer = layer->linkrecv(&datalen);

        if (datalen == 0)
        {
            return;
        }
        layer->recv(layer,pbuffer,datalen);
        return;
    }

    layer->down->unpack(layer->down);
}

/********************************************************
 * function    :  Protocol_Application_Getsize
 * Description :  ��ȡ���������ݵĴ�С
 * param       :  layer  Э������
 * return      :  ���������ݴ�С
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned int Protocol_Application_Getsize(PProtocol_Layer_ST layer)
{
    int ret = 0;
    PDevice_Collect_ST Pdeviceinfo;
    int index = 0;
    ret = layer->datalen;
    if (layer->devicesize)
    {
        if (layer->deviceinfo)
        {
            for (index=0;index<layer->devicesize;index++)
            {
            
                Pdeviceinfo = layer->deviceinfo[index];
                ret +=  Pdeviceinfo->len;
            }
        }
    }
    
    return ret;
}

/********************************************************
 * function    :  Protocol_Application_DeviceFree
 * Description :  �ͷ�Ӧ���������豸�б�����
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_DeviceFree(PProtocol_Layer_ST layer)
{
    unsigned short index = 0;
    PDevice_Collect_ST Pdeviceinfo = 0;
    if (layer->deviceinfo)
    {
        index = layer->devicesize;
        while (index)
        {
            Pdeviceinfo = layer->deviceinfo[index-1];
            if (Pdeviceinfo)
            {
                if (Pdeviceinfo->deviceinfo)
                {
                    free(Pdeviceinfo->deviceinfo);
                }

                free(Pdeviceinfo);

            }
            layer->deviceinfo[index-1] = 0;
            index--;
        }
        free(layer->deviceinfo);
        layer->deviceinfo = 0;
        layer->devicesize = 0;
    }
}

/********************************************************
 * function    :  Protocol_Application_DeviceParamFree
 * Description :  �ͷ�Ӧ���������豸��������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_DeviceParamFree(PProtocol_Layer_ST layer)
{
    unsigned short index = 0;
    PDevice_Param_ST Pdeviceparam = 0;
    if (layer->deviceparam)
    {
        index = layer->devicesize;
        while (index)
        {
            Pdeviceparam = layer->deviceparam[index-1];
            if (Pdeviceparam)
            {
                if (Pdeviceparam->deviceid)
                {
                    free(Pdeviceparam->deviceid);
                }
                if (Pdeviceparam->params)
                {
                    free(Pdeviceparam->params);
                }

                free(Pdeviceparam);

            }
            index--;
        }
        free(layer->deviceparam);
        layer->deviceparam = 0;
        layer->devicesize = 0;
    }
}

/********************************************************
 * function    :  Protocol_Application_AddDevice
 * Description :  ��Ӧ��Э���������һ���豸��Ϣ
 * param       :  layer  Э������
 * param       :  data  �豸���ݻ���
 * param       :  datalen  ���ݳ���
 * return      :  0 ��ʾ�ɹ� С��0 ��ʾʧ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Application_AddDevice(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    int index;
    unsigned short datasize = 0;
    PDevice_Collect_ST pdevice = 0;
    if (!layer->deviceinfo)
    {
        layer->deviceinfo = (PDevice_Collect_ST*)malloc(DEVICE_NUM_MAX * sizeof(PDevice_Collect_ST));
        memset(layer->deviceinfo,0,DEVICE_NUM_MAX * sizeof(PDevice_Collect_ST));
    }
    ///�ȼ�鵱ǰ�豸�б��е������Ƿ��Ѿ��ﵽ��8K����󳤶�
    index =  layer->devicesize;
    while (index)
    {
        pdevice = layer->deviceinfo[index-1];
        datasize +=  pdevice->len;
        if (datasize + (datalen - 6) > DEVICE_DATA_MAX)
        {
            return -1;
        }
        index--;
    }

    ///����ԭ����ֻ�ж�ǰ���6���ֽ��Ƿ���ͬ�������ͬ�����жϵ�ǰ�Ƿ��ܹ�֧�ֲ���

    if (layer->devicesize == 0)
    {
create_new:
        layer->deviceinfo[layer->devicesize] = (PDevice_Collect_ST)malloc(sizeof(Device_Collect_ST));
        memset(layer->deviceinfo[layer->devicesize],0,sizeof(Device_Collect_ST));
        layer->deviceinfo[layer->devicesize]->deviceinfo = (Pchar)malloc(datalen+2);
        layer->deviceinfo[layer->devicesize]->deviceinfo[0] = (char)((unsigned char)PT_DEVICE & 0xFF);
        layer->deviceinfo[layer->devicesize]->deviceinfo[1] = (char)(datalen & 0xFF);
        memcpy(&layer->deviceinfo[layer->devicesize]->deviceinfo[2],data,datalen);
        layer->deviceinfo[layer->devicesize]->len = datalen+2;
        layer->devicesize++;
        return 0;
    }

    index =  layer->devicesize;

    while(index)
    {
        pdevice = layer->deviceinfo[index-1];
        if (!memcmp(&pdevice->deviceinfo[2],data,6))
        {
            if (pdevice->len + (datalen - 6) > 255)
            {
                ///��ʾ��ǰ�����Ѿ����ˣ���Ҫ��������
                index--;
                continue;
            }
            ///��ʾ�ҵ�����ͬ�����ݣ�����Ҫ���������ӵ����е�β��
            pdevice->deviceinfo = (Pchar)realloc(pdevice->deviceinfo,pdevice->len + datalen);
            memcpy(&pdevice->deviceinfo[pdevice->len],&data[6],datalen-6);
            pdevice->len += datalen - 6;
            pdevice->deviceinfo[1] = (char)((pdevice->len -2) & 0xFF);
            return 0;
        }
        index--;
    }
    ///��ʾû���ҵ�������֮ǰ�Ķ����Ѿ����ˣ���Ҫ����һ��
   goto  create_new;
}

/********************************************************
 * function    :  Protocol_Application_PutTag
 * Description :  �������ݵ�Ӧ��Э�������
 * param       :  layer  Э������
 * param       :  tag  Ӧ�����ݱ�ǩ
 * param       :  data  ����
 * param       :  datalen  ���ݳ���
 * return      :  0 ��ʾ�ɹ� С��0 ��ʾʧ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
/*
static int Protocol_Application_PutTag(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen)
{
 
    #define PACK_SIZE (256)

    if (!layer->data)
    {
        layer->data = (Pchar)malloc(PACK_SIZE);
        memset(layer->data,0,PACK_SIZE);
        layer->datalen = 0;
    }
    #undef PACK_SIZE

    if (tag != PT_DEVICE)
    {
        switch (tag)
        {
            case PT_ACCOUNT :
                strcat((Pchar)&layer->data[1],(Pchar)"\"account\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_PASSWORD :
                strcat((Pchar)&layer->data[1],(Pchar)"\"password\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_DATE :
                strcat((Pchar)&layer->data[1],(Pchar)"\"date\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_TIME :
                strcat((Pchar)&layer->data[1],(Pchar)"\"time\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_MODEL :
                strcat((Pchar)&layer->data[1],(Pchar)"\"model\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_SOFTVERSION :
                strcat((Pchar)&layer->data[1],(Pchar)"\"softversion\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_HEARTBEAT :
                strcat((Pchar)&layer->data[1],(Pchar)"\"heartbeat\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;

        }
        layer->datalen = strlen((Pchar)&layer->data[1]);
        return 0;
    }
    else
    {
        ///���뵽�豸�б��н��й���
        return Protocol_Application_AddDevice(layer,data,datalen);
    }


}
*/

static int Protocol_Application_PutTag(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,unsigned char* data,unsigned short datalen)
{
 
    #define PACK_SIZE (256)

    if (!layer->data)
    {
        layer->data = (Pchar)malloc(PACK_SIZE);//layer->data = (Pchar)MALLOC(PACK_SIZE);
        memset(layer->data,0,PACK_SIZE);
        layer->datalen = 0;
    }
    #undef PACK_SIZE

    if (tag != PT_DEVICE)
    {
        switch (tag)
        {
            case PT_ACCOUNT :
                strcat((Pchar)&layer->data[1],(Pchar)"\"account\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_PASSWORD :
                strcat((Pchar)&layer->data[1],(Pchar)"\"password\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_DATE :
                strcat((Pchar)&layer->data[1],(Pchar)"\"date\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_TIME :
                strcat((Pchar)&layer->data[1],(Pchar)"\"time\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_MODEL :
                strcat((Pchar)&layer->data[1],(Pchar)"\"model\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_SOFTVERSION :
                strcat((Pchar)&layer->data[1],(Pchar)"\"softversion\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_HEARTBEAT :
                strcat((Pchar)&layer->data[1],(Pchar)"\"heartbeat\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;
            case PT_APPARAMVER:
                strcat((Pchar)&layer->data[1],(Pchar)"\"apparamver\":\"");
                strcat((Pchar)&layer->data[1],(Pchar)data);
                strcat((Pchar)&layer->data[1],(Pchar)"\",");
                break;

        }
        layer->datalen = strlen((Pchar)&layer->data[1]);
        return 0;
    }
    else
    {
        ///????????????
        return Protocol_Application_AddDevice(layer,data,datalen);
    }


}


/********************************************************
 * function    :  Protocol_Application_GetTag
 * Description :  ����Ӧ�ñ�ǩ��Ӧ�����ݻ����ж�ȡ����
 * param       :  layer  Э������
 * param       :  tag     Ӧ�����ݱ�ǩ
 * param       :  buffer  Ӧ�����ݻ���ָ��
 * return      :  Ӧ�����ݻ����С
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static int Protocol_Application_GetTag(PProtocol_Layer_ST layer,Protocol_Tag_EM tag,void** buffer)
{
    #if 0
    if (layer->down)
    {
        return layer->down->gettag(layer->down,tag,buffer);
    }
    #endif
    *buffer = (void*)layer->deviceparam;
    return (int)layer->devicesize;


}

/********************************************************
 * function    :  Protocol_Application_FillData
 * Description :  ������������
 * param       :  layer  Э������
 * param       :  index  ������ݻ���ƫ��
 * param       :  data  ���ݻ���
 * param       :  datalen  ���ݻ����С
 * return      :  ���������ݵ�ʵ�ʴ�С
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Application_FillData(PProtocol_Layer_ST layer,unsigned short index,unsigned char* data,unsigned short datalen)
{
    unsigned short __devicesize = 0;
    PDevice_Collect_ST Pdeviceinfo = 0;
    unsigned short __datalen = 0;
    unsigned short ret = 0;
    if (!layer->deviceinfo)
    {
        return 0;
    }

    if (!layer->devicesize)
    {
        return 0;
    }


    while (__devicesize < layer->devicesize)
    {
        Pdeviceinfo = layer->deviceinfo[__devicesize];
        if (index >= Pdeviceinfo->len)
        {
            index -= Pdeviceinfo->len;
        }
        else
        {
            if (Pdeviceinfo->len > datalen)
            {
                __datalen = datalen;
            }
            else
            {
                __datalen = Pdeviceinfo->len;
                if (index)
                {
                    __datalen -= index;
                }
            }
            memcpy((void*)data,(void*)&Pdeviceinfo->deviceinfo[index],__datalen);
            data += __datalen;
            datalen -= __datalen;
            ret += __datalen;
            index = 0;
        }

        if (!datalen)
        {
            break;
        }
        __devicesize++;
    }

    return ret;
}

///������������ݣ��ֿ齫���ݷ��ظ��²�
/********************************************************
 * function    :  Protocol_Application_Get
 * Description :  ��ȡ����������
 * param       :  layer  Э������
 * param       :  index  ������ݻ���ƫ��
 * param       :  data  ���ݻ���
 * param       :  datalen  ���ݻ����С
 * return      :  ���������ݵ�ʵ�ʴ�С
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Application_Get(PProtocol_Layer_ST layer,unsigned short index,unsigned char* data,unsigned short datalen)
{
    unsigned short ret = 0;
    unsigned short fillret = 0;
   

    ///������ݵ�ԭ���ǣ��Ƚ�layer->data�е�������䵽data�У�Ȼ���豸�б��е�������䵽data��
    /*
      1���ȼ���index��Ӧ�������е���ʼλ��
      2��Ȼ������λ��˳��Ŀ������ݵ�data��
      3������Ƿ������ݵĳ���
    */

    if (index < layer->datalen)
    {
        ///�ȴ�data���֣������ݿ�����data��
        if (layer->datalen - index > datalen)
        {
            ret = datalen;
        }
        else
        {
            ret = layer->datalen;
        }
        memcpy((void*)data,(void*)&layer->data[index],ret);

        datalen -= ret;
        data += ret;
        index = 0;
    }


    ///��Ҫ���豸�б����������
    fillret = Protocol_Application_FillData(layer,index,data,datalen);
    ret += fillret;

    return ret;
}

/********************************************************
 * function    :  Protocol_Application_Clear
 * Description :  ���Э��ջ����
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_Clear(PProtocol_Layer_ST layer)
{

    memset(layer->ParamList,0,(int)PP_MAX * sizeof(Pchar));

    if (layer->data)
    {
        free(layer->data);
        layer->data = 0;
    }
    Protocol_Application_DeviceParamFree(layer);
    Protocol_Application_DeviceFree(layer);

    if (!layer->down)
    {
        return;
    }

    layer->down->clear(layer->down);
}

/********************************************************
 * function    :  Protocol_Application_SetParam
 * Description :  ���ò���
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * param       :  value  ����ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_SetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param,Pchar value)
{
    if (param > PP_MAX || value == 0)
    {
        return;
    }

    layer->ParamList[param] = value;

    if (!layer->down)
    {
        return;
    }
    layer->down->setparam(layer->down,param,value);
}

/********************************************************
 * function    :  Protocol_Application_GetParam
 * Description :  ��ȡ����
 * param       :  layer  Э������
 * param       :  param  ������ǩ
 * return      :  ���ز���ֵָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static Pchar Protocol_Application_GetParam(PProtocol_Layer_ST layer,Protocol_Param_EM param)
{
    if (!layer->down)
    {
        return 0;
    }
    return layer->down->getparam(layer->down,param);
}

/********************************************************
 * function    :  Protocol_Application_Send
 * Description :  ��������
 * param       :  layer  Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_Send(PProtocol_Layer_ST layer)
{
    unsigned short index = 0;
    PDevice_Collect_ST Pdeviceinfo = 0;
    ///ִ�з��ʹ���
    if (layer->linksend)
    {
        ///�ȷ���data���ֵ�����
        layer->crc = crc_ccitt((unsigned char*)layer->data,strlen(layer->data),layer->crc);
        layer->linksend((unsigned char*)layer->data,layer->datalen);
        ///Ȼ�����豸�б��е�����
        if (layer->deviceinfo)
        {
            index = layer->devicesize;
            while (index--)
            {
                Pdeviceinfo = layer->deviceinfo[index-1];
                ///send֮ǰ�ȼ���crc16
                layer->crc = crc_ccitt((unsigned char*)layer->data,strlen(layer->data),layer->crc);
                layer->linksend((unsigned char*)Pdeviceinfo->deviceinfo,Pdeviceinfo->len);
            }
        }
    }


    if (layer->up)
    {
        layer->up->setcrc(layer->up,layer->crc);
        layer->up->send(layer->up);
    }
}

#define DEVICE_ID  "\"device_id\":"
#define DEVICE_PARAM  "\"paramlist\":"
/********************************************************
 * function    :  Protocol_Application_DeviceParamValue
 * Description :  ��ȡ�豸����ֵ
 * param       :  name ������
 * param       :  data ���ݻ���
 * param       :  datalen ���ݻ����С
 * return      :  ����ֵ��ָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned char* Protocol_Application_DeviceParamValue(unsigned char* name,unsigned char* data,unsigned short datalen)
{
    unsigned char* ret = 0;
    unsigned char* pret = 0;
    unsigned short namelen = strlen((Pchar)name);
    if (memcmp((void*)data,(void*)name,namelen))
    {
        return pret;
    }

    ret = malloc(datalen-namelen - 2);
    if (!ret)
    {
        return ret;	
    }
    memset((void*)ret,0,datalen-namelen - 2);
    pret = ret;
    data += namelen;
    datalen -= namelen;

    while(*data != '\"')
    {
        datalen--;
        data++;
    }
    data++;

    while(*data != '\"')
    {
        *pret = *data;
        datalen--;
        data++;
        pret++;
    }

    return ret;

}

/********************************************************
 * function    :  Protocol_Application_AddDeviceParam
 * Description :  ����Ӧ����������һ���豸����
 * param       :  layer  Э������
 * param       :  data  ���ݻ���
 * param       :  datalen  ���ݻ����С
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_AddDeviceParam(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    unsigned char* line;
    unsigned char* value;
    unsigned char* basestr = 0;
    unsigned short sectionsize = 0;
    layer->deviceparam = realloc(layer->deviceparam,(layer->devicesize+1) * sizeof(PDevice_Param_ST));
    layer->deviceparam[layer->devicesize] = malloc(sizeof(Device_Param_ST));
    memset(layer->deviceparam[layer->devicesize++],0,sizeof(Device_Param_ST));
    data =  protocol_Application_Section(data,datalen,&line);

    while(datalen)
    {
        sectionsize = data - line;

        value = Protocol_Application_DeviceParamValue((unsigned char*)DEVICE_ID,line,sectionsize);
        if (value)
        {
            layer->deviceparam[layer->devicesize - 1]->deviceid = (Pchar)value;
        }
        else
        {
            value = Protocol_Application_DeviceParamValue((unsigned char*)DEVICE_PARAM,line,sectionsize);
            if (value)
            {
                basestr = protocol_base64_decode(value);
                layer->deviceparam[layer->devicesize - 1]->paramlen = (strlen((Pchar)value) /4 * 3);
                memcpy((void*)value,(void*)basestr,layer->deviceparam[layer->devicesize - 1]->paramlen);
                layer->deviceparam[layer->devicesize - 1]->params = (Pchar)value;
                free(basestr);
            }
        }


        datalen -= sectionsize;
        data =  protocol_Application_Section(data,datalen,&line);

        if (*data == '}')
        {

            return;
        }
    }

}


#define FIND_NEXT_TAG(pdata,sdatalen,tag)  \
     do{                                                         \
     while(*pdata != tag && sdatalen > 0){pdata++;sdatalen--;}   \
     if (!sdatalen){return pdata;}                               \
     break;                                                      \
     }while(0)

#define FIND_NEXT_2TAG(pdata,sdatalen,tag1,tag2)  \
     do{                                                         \
     while((*pdata != tag1 && *pdata != tag2) && sdatalen > 0){pdata++;sdatalen--;}   \
     if (!sdatalen){return pdata;}                               \
     break;                                                      \
     }while(0)

#define FIND_NEXT_TAG1(pdata,sdatalen,tag)  \
     do{                                                         \
     while(*pdata != tag && sdatalen > 0){pdata++;sdatalen--;}   \
     if (!sdatalen){return;}                               \
     break;                                                      \
     }while(0)

#define FIND_BACKWARD_TAG1(pdata,sdatalen,tag)  \
     do{                                                         \
     while(*pdata != tag && sdatalen > 0){pdata--;sdatalen--;}   \
     if (!sdatalen){return;}                               \
     break;                                                      \
     }while(0)

/********************************************************
 * function    :  protocol_Application_Section
 * Description :  ��ȡһ����Ч���ݶ�
 * param       :  data  �������ݻ���
 * param       :  datalen  Э������
 * param       :  line  ���ݶε���ʼָ��
 * return      :  ���ջ��棬ָ����һ�����ݶε�ָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned char* protocol_Application_Section(unsigned char* data,unsigned short datalen,unsigned char** line)
{

    int tag_type = 0;
    int tag_deep = 0;
    *line = data;
    FIND_NEXT_TAG(data,datalen,'\"');
    *line = data;
    data++;
    FIND_NEXT_TAG(data,datalen,'\"');
    data++;
    FIND_NEXT_TAG(data,datalen,':');
    data++;

    while(datalen)
    {
        if (*data == '{')
        {
            tag_type = 0;
            data++;
            tag_deep++;
            break;
        }
        else
        if (*data == '[')
        {
            tag_type = 1;
            data++;
            tag_deep++;
            break;
        }
        else if (*data == '\"')
        {
            tag_type = 2;
            data++;
            tag_deep++;
            break;
        }
        else if (*data == 'n')
        {
            tag_type = 3;
            break;
        }
        data++;
        datalen--;
    }

    if (!datalen){return data;}
    while(datalen)
    {
        switch (tag_type)
        {
            case 0: //��ʾ�Ƕ�������
                  if (*data == '{')
                  {
                      tag_deep++;
                  }
                  else if (*data == '}')
                  {
                      tag_deep--;
                  }
                  break;

            case 1: //��ʾ����������
                  if (*data == '[')
                  {
                      tag_deep++;
                  }
                  else if (*data == ']')
                  {
                      tag_deep--;
                  }
                  break;
            case 2: //��ʾ���ַ�������
                  if (*data == '\"')
                  {
                      tag_deep--;
                  }
                  break;
            case 3: ///��ʾ�ǿ�����ָ��
                  if (!memcmp((void*)data,(void*)"null",4))
                  {
                      tag_deep = 0;
                      datalen -= 3;
                      data += 3;
                  }
                  break;
        }

        data++;
        datalen--;
        if (!tag_deep)
        {
             break;
        }
    }
    if (!datalen){return data;}
    FIND_NEXT_2TAG(data,datalen,',','}');
    if (*data == ',')
    {
        data++;
    }
    return data;
}

typedef enum tag_type_em
{
    TT_ELEMENT,
    TT_OBJECT,
    TT_ARRAY
}Tag_Type_EM;

struct paramtag_st
{
    Pchar    param;
    Tag_Type_EM       type;
    Protocol_Param_EM tag;
};

static struct paramtag_st paramlist[9] = {
                                      {"\"sessionid\":",TT_ELEMENT,PP_SESSIONID},
                                      {"\"sessionkey\":",TT_ELEMENT,PP_SESSIONKEY},
                                      {"\"resultcode\":",TT_ELEMENT,PP_RESULTCODE},
                                      {"\"url\":",TT_ELEMENT,PP_URL},
                                      {"\"version\":",TT_ELEMENT,PP_SOFTVERSION},
                                      {"\"apparam\":",TT_ELEMENT,PP_APPARAM},
									  {"\"apparamver\":",TT_ELEMENT,PP_APPARAMVER},
                                      {"\"devicelist\":",TT_ARRAY,PP_MAX},
                                      {"\"apinfo\":",TT_OBJECT,PP_MAX}
                                      };


/********************************************************
 * function    :  Protocol_Application_GetParamValue
 * Description :  ��ȡ�豸����ֵ
 * param       :  param  ��������
 * param       :  datalen  ���������С
 * param       :  value  ����ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :  2015-07-03 �����������Ϊ�յ�����

*/
static void Protocol_Application_GetParamValue(unsigned char* param,unsigned short datalen,unsigned char* value)
{
    unsigned char* Pbuffer = param;
    while(*Pbuffer == '"' && datalen > 0)
    {
        Pbuffer++;
        datalen--;
    }
    
    if ((Pbuffer - param) == 2)
    {
        return;
    }
    if (Pbuffer == param)
    {
        while(*Pbuffer != ',' && datalen > 0)
        {
            *value++ = *Pbuffer++;
            datalen--;
        }
    }
    else
    {
        while(*Pbuffer != '"' && datalen > 0)
        {
            *value++ = *Pbuffer++;
            datalen--;
        }
    }

}

/********************************************************
 * function    :  protocol_Application_ArrayItem
 * Description :  �����������
 * param       :  layer  Э������
 * param       :  tag  Ӧ�ö����ǩ
 * param       :  data  ���ݻ���
 * param       :  datalen  ���ݻ����С
 * param       :  line  Ӧ�ñ�ǩָ��
 * return      :  ָ����һ�����ݶε�ָ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned char* protocol_Application_ArrayItem(PProtocol_Layer_ST layer,unsigned char* tag,unsigned char* data,unsigned short datalen,unsigned char** line)
{
    *line = data;
    FIND_NEXT_TAG(data,datalen,'{');
    *line = data;
    if (!memcmp((void*)tag,(void*)"\"devicelist\":",strlen("\"devicelist\":")))
    {
        Protocol_Application_AddDeviceParam(layer,data,datalen);
    }

    FIND_NEXT_TAG(data,datalen,'}');
    FIND_NEXT_TAG(data,datalen,',');
    return ++data;
}

/********************************************************
 * function    :  Protocol_Application_ObjectParse
 * Description :  �������
 * param       :  layer  Э������
 * param       :  data  ���ݻ���
 * param       :  datalen  ���ݻ����С
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_ObjectParse(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    //unsigned char* line;
    unsigned char* pbuffer = data + datalen;
    FIND_NEXT_TAG1(data,datalen,'{');
    data++;
    datalen--;
    FIND_BACKWARD_TAG1(pbuffer,datalen,'}');

    if (!datalen)
    {
        return;
    }

    Protocol_Application_Parse(layer,data,datalen);



    #if 0
    data =  protocol_Application_Section(data,datalen,&line);

    while(datalen)
    {



        datalen -= data - line;
        data =  protocol_Application_Section(data,datalen,&line);
    }
    #endif

}

/********************************************************
 * function    :  Protocol_Application_ArrayParse
 * Description :  �������
 * param       :  layer  Э������
 * param       :  tag  Ӧ�����ݱ�ǩ
 * param       :  data  ���ݻ���
 * param       :  datalen  ���ݻ����С
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_ArrayParse(PProtocol_Layer_ST layer,unsigned char* tag,unsigned char* data,unsigned short datalen)
{
    unsigned char* line;
    FIND_NEXT_TAG1(data,datalen,'[');
    data++;
    datalen--;
    if (!datalen)
    {
        return;
    }
    data =  protocol_Application_ArrayItem(layer,tag,data,datalen,&line);

    while(datalen)
    {

        datalen -= data-line;
        data =  protocol_Application_ArrayItem(layer,tag,data,datalen,&line);
    }

}

/********************************************************
 * function    :  Protocol_Application_SessionKey
 * Description :  �����Ự��Կ
 * param       :  layer  Э������
 * param       :  in  ����ǰ�ĻỰ��Կ
 * param       :  out  ������ĻỰ��Կ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_SessionKey(PProtocol_Layer_ST layer,unsigned char* in,unsigned char* out)
{
	int i = 0;
    unsigned char* basestr = 0;
    int basestrlen = 0;

	printf("protocol_base64_decode(in) = %s\n",in);
    basestr = protocol_base64_decode(in);
	printf("basestr = %s\n",basestr);
    if (!basestr)
    {
		printf("if (!basestr)\n");
        return;
    }
    basestrlen = strlen((Pchar)in);
    basestrlen = basestrlen / 4 * 3;
    basestrlen = basestrlen /8 * 8;
    memcpy((void*)out,(void*)basestr,basestrlen);
    out[basestrlen] = 0;
	
	for(i = 0; i < basestrlen; i++)
	{
		printf("%02X ",basestr[i]);
	}
	
	printf("\nbasestrlen  1= %d\n",basestrlen);
    protocol_tripledes((unsigned char*)layer->protocol->getparam(layer->protocol,PP_PSDMD5),out,out,DECRYPT);
	printf("in = %s\n",in);
    out+=8;
    protocol_tripledes((unsigned char*)layer->protocol->getparam(layer->protocol,PP_PSDMD5),out,out,DECRYPT);
	printf("basestrlen  2= %d\n",basestrlen);
    free(basestr);
    return;
}

/********************************************************
 * function    :  Protocol_Application_ParamParse
 * Description :  ��������
 * param       :  layer  Э������
 * param       :  param  ��������
 * param       :  datalen  ���������С
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_ParamParse(PProtocol_Layer_ST layer,unsigned char* param,unsigned short datalen)
{
    int index;
    int size = sizeof(paramlist) / sizeof(struct paramtag_st);
    unsigned char value[64] = {0};


    for (index = 0;index<size;index++)
    {
        if (!memcmp(paramlist[index].param,param,strlen(paramlist[index].param)))
        {

            param += strlen(paramlist[index].param);
            datalen -= strlen(paramlist[index].param);
            if (paramlist[index].type == TT_ELEMENT)
            {
                memset(value,0,sizeof(value));
                Protocol_Application_GetParamValue(param,datalen,value);
				//printf("value : %s\n",value);
                if (paramlist[index].tag == PP_URL)
                {
                    //printf("PP_URL_kernel:%s\r\n",value); 
                }
                if (paramlist[index].tag == PP_SESSIONKEY)
                {
                    ///�Ự��Կ��Ҫ����base64����Ȼ���ٽ��н���
                    Protocol_Application_SessionKey(layer,value,value);
                   // printf("PP_SessionKey_kernel:%s\r\n",value); 
                }
                
                layer->protocol->setparam(layer->protocol,paramlist[index].tag,(Pchar)value);

            }
            else if (paramlist[index].type == TT_ARRAY)
            {
                Protocol_Application_ArrayParse(layer,(unsigned char*)paramlist[index].param,param,datalen);
            }
            else if (paramlist[index].type == TT_OBJECT)
            {
                Protocol_Application_ObjectParse(layer,param,datalen);
            }
            return;
        }
    }

}

/********************************************************
 * function    :  Protocol_Application_Parse
 * Description :  Ӧ��Э���ͷЭ����ʼ������
 * param       :  layer  Э������
 * param       :  data  ���ݻ���
 * param       :  datalen  ���������С
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_Parse(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    unsigned char* line;
    unsigned short sectionsize = 0;

    data =  protocol_Application_Section(data,datalen,&line);
	
    while(datalen)
    {
	   // printf("Protocol_Application_Parse %s\n",data);
        sectionsize = data - line;
		//printf("Protocol_Application_Parse sectionsize = %d\n",sectionsize);
        if (sectionsize > datalen)
        {
            return;
        }
		//printf("Protocol_Application_Parse line = %s\n",line);
        Protocol_Application_ParamParse(layer,line,sectionsize);
		//printf("line end\n");
        datalen -= sectionsize;
        data =  protocol_Application_Section(data,datalen,&line);
    }
}

/********************************************************
 * function    :  Protocol_Application_Recv
 * Description :  ���շ�����Ӧ��
 * param       :  layer  Э������
 * param       :  data  ���ݻ���
 * param       :  datalen  ���������С
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_Recv(PProtocol_Layer_ST layer,unsigned char* data,unsigned short datalen)
{
    //printf("application data:%s\r\n",data);
    Protocol_Application_Parse(layer,++data,datalen-2);
    //printf("application data end\r\n");
	  ///�ͷ�����
    if (layer->data)
    {
        free(layer->data);
        layer->data = 0;
    }
    Protocol_Application_DeviceFree(layer);
    
	
    if (layer->up)
    {
        layer->up->recv(layer->up,data,datalen);
    }
    layer->linkfree();
		

}

/********************************************************
 * function    :  Protocol_Application_GetCrc
 * Description :  ��ȡcrcУ��ֵ
 * param       :  layer  Э������
 * return      :  crcУ��ֵ
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static unsigned short Protocol_Application_GetCrc(PProtocol_Layer_ST layer)
{
    if (layer->up)
    {
        return layer->up->getcrc(layer->up);
    }

    return layer->crc;
}

/********************************************************
 * function    :  Protocol_Application_SetCrc
 * Description :  ����crcУ��ֵ
 * param       :  layer  Э������
 * param       :  crc  crcУ��ֵ
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
static void Protocol_Application_SetCrc(PProtocol_Layer_ST layer,unsigned short crc)
{

    layer->crc = ~crc;
}

/********************************************************
 * function    :  Protocol_Application_Initialize
 * Description :  ����Ӧ��Э������
 * param       :  Ӧ��Э������
 * return      :  ��
 * Author      :  ��
 * Update      :  2015-06-26
 * History     :

*/
PProtocol_Layer_ST Protocol_Application_Initialize(void)
{
    if (!Self)
    {
        Self = malloc(sizeof(*Self));
        memset(Self,0,sizeof(*Self));

        Self->init     = Protocol_Application_Init;
        Self->pack     = Protocol_Application_Pack;
        Self->unpack   = Protocol_Application_Unpack;
        Self->getsize  = Protocol_Application_Getsize;
        Self->puttag   = Protocol_Application_PutTag;
        Self->gettag   = Protocol_Application_GetTag;
        Self->clear    = Protocol_Application_Clear;
        Self->setparam = Protocol_Application_SetParam;
        Self->getparam = Protocol_Application_GetParam;
        Self->send     = Protocol_Application_Send;
        Self->recv     = Protocol_Application_Recv;
        Self->get      = Protocol_Application_Get;
        Self->getcrc   = Protocol_Application_GetCrc;
        Self->setcrc   = Protocol_Application_SetCrc;

        Self->ParamList = malloc((int)PP_MAX * sizeof(Pchar));
        memset(Self->ParamList,0,(int)PP_MAX * sizeof(Pchar));
    }
    return Self;
}


