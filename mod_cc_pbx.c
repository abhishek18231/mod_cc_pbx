/*
 * Freeswitch modular soft-switch application
 * All Rights Reserved © 2019 CloudConnect Communication Pvt. Ltd.
 *
 * The Initial Developer of the Original Code is
 * Ravindrakumar D. Bhatt <ravindra@cloud-connect.in>.
 * Contribution Event handler : Vivek Negi <vivek.negi@cloud-connect.in>.
 *
 * mod_cc_pbx.c --Implements broadcast based conference APP for outbound
 * trunk dailing
 *
 */
#include "cc_pbx.h"
//#include <maxminddb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>



#define xml_safe_free(_x) if (_x) switch_xml_free(_x); _x = NULL

SWITCH_MODULE_LOAD_FUNCTION(mod_cc_pbx_load);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_cc_pbx_shutdown);
SWITCH_MODULE_DEFINITION(mod_cc_pbx, mod_cc_pbx_load, mod_cc_pbx_shutdown, NULL);


static const char *global_cf = "cc_pbx.conf";

static struct {
    char *odbc_dsn;
    char *real_path;
    char* custom_path;
    char* opsip_ip_port;

    switch_mutex_t *mutex;
    switch_memory_pool_t *pool;

} globals;

//void event_handler(switch_event_t *event);

static switch_status_t load_config(void){
    switch_xml_t xml,cfg,settings,param;
    switch_status_t status = SWITCH_STATUS_SUCCESS;

    if(!(xml = switch_xml_open_cfg(global_cf,&cfg,NULL))){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"Open if %s failed.\n",global_cf);
        return SWITCH_STATUS_TERM;
    }



    if((settings = switch_xml_child(cfg,"settings"))){
       for(param = switch_xml_child(settings,"param");param;param = param->next){
            char *var = (char *) switch_xml_attr_soft(param, "name");
            char *val = (char *) switch_xml_attr_soft(param, "value");
            if(!strcasecmp(var,"odbc-dsn")){
                globals.odbc_dsn = strdup(val);
            }
            else if(!strcasecmp(var,"real_path")){
                globals.real_path = strdup(val);
            } 
            else if(!strcasecmp(var,"custom_prompt")){
                globals.custom_path = strdup(val);
            } 
            else if(!strcasecmp(var,"opsip_ip_port")){
                globals.opsip_ip_port = strdup(val);
            } 
            else{
                status = SWITCH_STATUS_TERM;
            }
       } 
    }

    xml_safe_free(xml);
    return status;
}






SWITCH_STANDARD_APP(cc_pbx_function){
    

    switch_status_t status = SWITCH_STATUS_SUCCESS;
    const char* dialstatus;
    char * temp_path=NULL;
    switch_channel_t * channel = switch_core_session_get_channel(session);
    //switch_call_cause_t cause = SWITCH_CAUSE_NORMAL_CLEARING;
 
 call_details_t call = {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};
    if( ( status = handle_call(channel,globals.odbc_dsn,globals.mutex,globals.real_path,globals.custom_path,globals.opsip_ip_port,&call ) ) != SWITCH_STATUS_SUCCESS ){

	switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
        return ;
    }

    // if callee is our  systems extension than only execute below logic for other type goto end
    dialstatus = switch_channel_get_variable(channel,"DIALSTATUS");
    //switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"Hangup cause: %s\n",dialstatus);  
    if( dialstatus!=NULL  &&!strcmp(dialstatus,"GATEWAY_DOWN")  ){
	            sleep(2);
                    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cause: %s\n",dialstatus);
                    temp_path = switch_mprintf("%s%s", globals.real_path, "gateway_down.wav");
                    switch_ivr_play_file(session, NULL, temp_path, NULL);
                    switch_safe_free(temp_path);
    }
     return;
}


SWITCH_MODULE_LOAD_FUNCTION(mod_cc_pbx_load)
{
    switch_application_interface_t *app_interface;
//  switch_api_interface_t *commands_api_interface;
    switch_status_t status;

    *module_interface = switch_loadable_module_create_module_interface(pool, modname);

    memset(&globals,0,sizeof(globals));
    globals.pool = pool;

    switch_mutex_init(&globals.mutex,SWITCH_MUTEX_NESTED,globals.pool);

    if( (status = load_config()) != SWITCH_STATUS_SUCCESS ){
        return status;
    }
 
  /*    if (switch_event_bind("mod_cc_pbx", SWITCH_EVENT_ALL , SWITCH_EVENT_SUBCLASS_ANY, event_handler,NULL) != SWITCH_STATUS_SUCCESS) {
   switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Cannot bind to event handler!\n"); 

		return SWITCH_STATUS_TERM;
	}  

   
   */

 
 SWITCH_ADD_APP(app_interface, "cc_pbx", "cc_pbx", "cc pbx application", cc_pbx_function, NULL, SAF_NONE);

    return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_cc_pbx_shutdown)
{

    //switch_event_unbind_callback(event_handler);
    switch_safe_free(globals.odbc_dsn);
    switch_safe_free(globals.real_path);
    switch_mutex_destroy(globals.mutex);
    return SWITCH_STATUS_SUCCESS;

}



 /*void  event_handler(switch_event_t *event) {  

     char *buf;
     
     switch_event_serialize(event, &buf, SWITCH_TRUE);
  	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, "\nEVENT (text version)\n--------------------------------\n%s", buf);
   
 }
 */
 



/*
void get_loc(const char *ip) {
    MMDB_s mmdb;
    int status;
    MMDB_lookup_result_s result;

    status = MMDB_open("/var/www/html/fs_backend/GeoLite2-City.mmdb", MMDB_MODE_MMAP, &mmdb);
    if (MMDB_SUCCESS != status) {
        fprintf(stderr, "Error opening GeoLite2 database: %s\n", MMDB_strerror(status));
        return;
    }

    if (strcmp(ip, "192.168.100.4") != 0) {
         
        status = MMDB_lookup_string(&mmdb, ip, &result);
        if (status == MMDB_SUCCESS) {
            MMDB_entry_data_s entry_data;
            int gai_error;

            memset(&entry_data, 0, sizeof(entry_data));
            if (MMDB_get_value(&result.entry, &entry_data, "latitude", "double", &gai_error) == MMDB_SUCCESS) {
                if (entry_data.has_data) {
                    double latitude = entry_data.double_value;
                    printf("Latitude: %f\n", latitude);
                }
            }

            memset(&entry_data, 0, sizeof(entry_data));
            if (MMDB_get_value(&result.entry, &entry_data, "longitude", "double", &gai_error) == MMDB_SUCCESS) {
                if (entry_data.has_data) {
                    double longitude = entry_data.double_value;
                    printf("Longitude: %f\n", longitude);
                }
            }
        } else {
            fprintf(stderr, "Error looking up IP address: %s\n", MMDB_strerror(status));
        }
    }

    MMDB_close(&mmdb);
}

 */




 void  event_handler(switch_event_t *event) {  
    


          #define QUERY_MAX_LENGTH 1024
          char *query = (char *)malloc(1000 * sizeof(char));
          const char *event_name = switch_event_get_header_nil(event, "Event-Name");
          char *country_code = switch_event_get_header_nil(event, "variable_dial_prefix"); 
          char *forward      = switch_event_get_header_nil(event, "Caller-Callee-ID-Number"); 
          char *call_plan  = switch_event_get_header_nil(event, "variable_call_plan_id"); 
          char *caller_read_codec  = switch_event_get_header_nil(event, "Channel-Read-Codec-Name"); 
          char *caller_write_codec  = switch_event_get_header_nil(event, "Channel-Write-Codec-Name"); 
                
          
          //const char *click2call_cdr = switch_event_get_header_nil(event, "Caller-Logical-Direction");
          const char *type_call = switch_event_get_header_nil(event, "Call-Direction");
          const char *call_type = switch_event_get_header_nil(event, "variable_call_type");
          const char *cust_id = switch_event_get_header_nil(event, "variable_cust_id");
          const char *core_uuid = switch_event_get_header_nil(event, "Channel-Call-UUID");
          const char *codec = switch_event_get_header_nil(event, "Channel-Write-Codec-Name");
          const char *ip = switch_event_get_header_nil(event, "variable_sip_h_X-userip");
          const char *application = switch_event_get_header_nil(event, "variable_application");
          const char *src = switch_event_get_header_nil(event, "Caller-Caller-ID-Number");
           char *dest = switch_event_get_header_nil(event, "Caller-Destination-Number");
          //const char *dial_prefix = switch_event_get_header_nil(event, "variable_dial_prefix");
          const char *gtwy = switch_event_get_header_nil(event, "variable_gateway_group_id");
          const char *ref_id = switch_event_get_header_nil(event, "variable_ref_id");
          const char *sip_contact_uri = switch_event_get_header_nil(event, "variable_sip_contact_uri");
          const char *sip_call_id = switch_event_get_header_nil(event, "variable_sip_call_id");
          const char *cle_uuid = (strcmp(call_type, "click2call") == 0 || strcmp(call_type, "patch_api") == 0) ? switch_event_get_header_nil(event, "variable_job_uuid_b_leg") : switch_event_get_header_nil(event, "Bridge-B-Unique-ID");
          const char *uuid = (strcmp(call_type, "click2call") == 0 || strcmp(call_type, "patch_api") == 0 || strcmp(call_type, "plugin") == 0) && switch_event_get_header_nil(event, "variable_job_uuid") != NULL ? switch_event_get_header_nil(event, "variable_job_uuid") : switch_event_get_header_nil(event, "Unique-ID");
          //const char *hangup = switch_event_get_header_nil(event, "variable_hangup_cause");
          //const char *record_file = switch_event_get_header_nil(event, "variable_recording_file");
          const char *id_callplan = switch_event_get_header_nil(event, "variable_call_plan_id");
          const char *minute_plan_id = switch_event_get_header_nil(event, "variable_minute_plan_id");
          //const char *clr_name = switch_event_get_header_nil(event, "variable_clr_name");
          //const char *cle_name = switch_event_get_header_nil(event, "variable_cle_name");
          const char *sell_billing_blk = strcmp(switch_event_get_header_nil(event, "variable_selling_billing_block"), "NONE") != 0 ? switch_event_get_header_nil(event, "variable_selling_billing_block") : "0";
          const char *buy_cost = switch_event_get_header_nil(event, "variable_buy_rate") != NULL ? switch_event_get_header_nil(event, "variable_buy_rate") : "0";
          const char *sell_cost = switch_event_get_header_nil(event, "variable_sell_rate") != NULL ? switch_event_get_header_nil(event, "variable_sell_rate") : "0";
          //const char *Event_Subclass = switch_event_get_header_nil(event, "Event-Subclass");
          //const char *dst_id = switch_event_get_header_nil(event, "variable_dst_id");
          //const char *active_feature_id = switch_event_get_header_nil(event, "variable_active_feature_id");
          const char *trunk_id = switch_event_get_header_nil(event, "variable_sip_h_X-trunk_id");




 


   /*--------------------------------------------------------------------CHANNEL_CREATE --------------------------------------------------*/
    if (!strcasecmp(event_name,"CHANNEL_CREATE") ){

    char *call_back_plugin = switch_event_get_header_nil(event, "variable_plugin_data");
    const char *dail_prefix = switch_event_get_header_nil(event, "variable_dial_prefix");
    const char *custom_uuid = switch_event_get_header_nil(event, "variable_custom_uuid");
    //const char *esl_country_code = switch_event_get_header_nil(event, "variable_sip_h_X-userip");
    int num_tokens = 0;
    const char *delimiter = "_";
    char  result[20] = {'\0'}; 
    char *plugin_data[3];
    //const char *plugin_name = NULL;


   /*  if (strcmp(esl_country_code, "None") != 0) {
        char *roaming_ip = get_roaming_ip(esl_country_code, src);
        if (roaming_ip != NULL) {
            esl_country_code = roaming_ip;
        }
    }
 */
   
   


   if (call_back_plugin != NULL) {
   
    char *token = strtok(call_back_plugin, delimiter);
    while (token != NULL && num_tokens < 3 - 1) {
        plugin_data[num_tokens++] = token;
        token = strtok(NULL, delimiter);
    }
    plugin_data[num_tokens] = NULL; 
} else {
    
    const char *dest = switch_event_get_header_nil(event, "Caller-Destination-Number");
    if (dest != NULL) {
        char dest_copy[strlen(dest) + 1];
        char *token = strtok(dest_copy, delimiter);
        strcpy(dest_copy, dest);
        while (token != NULL && num_tokens < 3 - 1) {
            plugin_data[num_tokens++] = token;
            token = strtok(NULL, delimiter);
        }
        plugin_data[num_tokens] = NULL; 
    }
}

   
   
    

    //if ( plugin_data != NULL && plugin_data[0] != NULL && strcmp(plugin_data[0], "plugin") == 0) { -----Abhishek changes
    if (plugin_data[0] != NULL && strcmp(plugin_data[0], "plugin") == 0){
        if (strstr(plugin_data[1], "sip") != NULL) {
         snprintf(query, QUERY_MAX_LENGTH, "SELECT ext_number FROM extension_master WHERE id=%s", plugin_data[2]);
         execute_sql2str(globals.odbc_dsn, globals.mutex, query, result, NELEMS(result));
          if (atoi(result) != 0){
           snprintf(dest, QUERY_MAX_LENGTH, "Plugin Extension (%s)", result);
          }
        } else if (strstr(plugin_data[1], "ivr") != NULL) {
        snprintf(query, QUERY_MAX_LENGTH, "SELECT name FROM pbx_ivr_master WHERE id=%s", plugin_data[2]);
         execute_sql2str(globals.odbc_dsn, globals.mutex, query, result, NELEMS(result));
          if (atoi(result) != 0){
           snprintf(dest, QUERY_MAX_LENGTH, "Plugin IVR (%s)", result);
          }

        } else if (strstr(plugin_data[1], "cg") != NULL) {
           snprintf(query, QUERY_MAX_LENGTH, "SELECT name FROM pbx_callgroup WHERE id=%s", plugin_data[2]);
         execute_sql2str(globals.odbc_dsn, globals.mutex, query, result, NELEMS(result));
          if (atoi(result) != 0){
           snprintf(dest, QUERY_MAX_LENGTH, "Plugin Call Group (%s)", result);
          }

        } else if (strstr(plugin_data[1], "queue") != NULL) {
            snprintf(query, QUERY_MAX_LENGTH, "SELECT name FROM pbx_queue WHERE id=%s", plugin_data[2]);
         execute_sql2str(globals.odbc_dsn, globals.mutex, query, result, NELEMS(result));
          if (atoi(result) != 0){
           snprintf(dest, QUERY_MAX_LENGTH, "Plugin Queue (%s)", result);
          }

        } else if (strstr(plugin_data[1], "contact") != NULL) {
             snprintf(dest, QUERY_MAX_LENGTH, "Plugin Contact (%s)", plugin_data[2]);
           
        }
    }


    if (strcmp(type_call, "inbound") == 0 || (call_type != NULL && strcmp(type_call, "outbound") == 0)) {

     snprintf(query, QUERY_MAX_LENGTH,
             "INSERT INTO pbx_realtime_cdr(uuid, src, dst, ip, current_status, callerid, sip_call_id,destination, direct_gateway, customer_id, call_type, ref_id, sip_current_application,"
             "sip_contact_uri, custom_uuid, cle_uuid) VALUES('%s', '%s', '%s', '%s', '%s', '%s','%s', '%s', '%s', '%s','%s', '%s', '%s', '%s', '%s','%s')",
             uuid, src, dest, ip, event_name, src, sip_call_id, dail_prefix, gtwy, cust_id, type_call,ref_id, call_type, sip_contact_uri, custom_uuid, cle_uuid);

   
    execute_sql(globals.odbc_dsn, query, globals.mutex);
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " create  event query:%s  \n",query);
    switch_safe_free(query);
    }

    }
    
   /*--------------------------------------------------------------------CHANNEL_ANSWER --------------------------------------------------*/
 



   if (!strcasecmp(event_name,"CHANNEL_ANSWER")){



if (strcmp(type_call, "inbound") == 0 || (call_type != NULL && strcmp(type_call, "outbound") == 0)) {
     

    
    if (strcmp(type_call, "inbound") == 0 && strcmp(application, "intercom") == 0) {
        snprintf(query, QUERY_MAX_LENGTH, "update pbx_realtime_cdr set current_status='%s', codec='%s', call_type='%s',"
                 " sip_current_application='%s', customer_id='%s', clr_read_codec='%s', sell_cost='%s', ref_id='%s',"
                 " clr_write_codec='%s', minute_plan_id='%s', direct_gateway='%s' where uuid='%s'",
                 event_name, codec, application, call_type, cust_id, caller_read_codec, sell_cost, ref_id,
                 caller_write_codec, minute_plan_id, gtwy, core_uuid);
        
        execute_sql(globals.odbc_dsn, query, globals.mutex);
        //switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " answ  event query:%s  \n",query);

         
    }
    
    if (strcmp(type_call, "inbound") == 0 && strcmp(application, "outbound") == 0) {
        char  did_no[20] = {'\0'};
        char  vmn_dest[20] = {'\0'};

        snprintf(query, QUERY_MAX_LENGTH,"call switch_verify_did_number('%s')", dest);
        execute_sql2str(globals.odbc_dsn, globals.mutex, query, did_no, NELEMS(did_no));
       
        if (atoi(did_no) > 0) {
           
            snprintf(query, QUERY_MAX_LENGTH,"switch_vmn_cli_manipulation('%s')", did_no);
            execute_sql2str(globals.odbc_dsn, globals.mutex, query, vmn_dest, NELEMS(vmn_dest));
            
            if (vmn_dest[0] != '\0' && strlen(vmn_dest) > 0) {
                dest = vmn_dest;
            }

            /*-----Abhishek changes*/
            // if (vmn_dest != NULL && strlen(vmn_dest) > 0) {
                
            //     dest = vmn_dest;
            // }
        }
        
        snprintf(query, QUERY_MAX_LENGTH, "update pbx_realtime_cdr set current_status='%s', dst='%s', ip_internal='%s',"
                 " buy_cost='%s', sell_cost='%s', selling_billing_block='%s', id_callplan='%s', codec='%s',"
                 " direct_gateway='%s', call_type='%s', sip_current_application='%s', clr_read_codec='%s', ref_id='%s',"
                 " clr_write_codec='%s', minute_plan_id='%s', direct_gateway='%s' where uuid='%s'",
                 event_name, dest, switch_event_get_header_nil(event, "variable_local_media_ip"),
                 buy_cost, sell_cost, sell_billing_blk, id_callplan, codec, gtwy, application, call_type,
                 caller_read_codec, ref_id, caller_write_codec, minute_plan_id, gtwy, cle_uuid);
         execute_sql(globals.odbc_dsn, query, globals.mutex);

      //switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " answ  event query:%s  \n",query);



    }
}


   switch_safe_free(query);

    }
    


    
     /*--------------------------------------------------------------------CHANNEL_BRIDGE --------------------------------------------------*/

    if (!strcasecmp(event_name,"CHANNEL_BRIDGE")){

       


  
      
    if ( !strcmp(type_call,"inbound")  || !(strcmp(call_type,"click2call")  && !strcmp(type_call,"outbound") )){


    }

     if ( !strcmp(call_type,"outbound") || !strcmp(call_type,"click2call") ){

         query  = switch_mprintf("update pbx_realtime_cdr set \
             direct_gateway='%s',cle_uuid='%s' ,buy_cost='%s',sell_cost='%s',\
             id_callplan='%s' , destination='%s' ,current_status='%s'  ,forward ='%s',ip_internal ='%s',\
             sip_current_application ='%s',customer_id='%s',clr_read_codec='%s'\
             ,clr_write_codec='%s' where uuid='%s'",  
                    gtwy,
                    cle_uuid,
                    buy_cost,
                    sell_cost,
                    call_plan,
                    country_code,
                    event_name,
                    forward,
                    ip,
                    call_type,
                    cust_id,
                    caller_read_codec,
                    caller_write_codec,
                    core_uuid);
             execute_sql(globals.odbc_dsn, query, globals.mutex);
             switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " Bridge  event query:%s  \n",query);
             switch_safe_free(query);
     }
           
      else if (!strcasecmp(application,"inbound") || !strcasecmp(application,"intercom")){

         query  = switch_mprintf("update pbx_realtime_cdr set \
             cle_uuid='%s',current_status='%s'  ,forward ='%s',\
             sip_current_application ='%s',buy_cost='%s',sell_cost='%s',customer_id='%s',clr_read_codec='%s'\
             ,clr_write_codec='%s' where uuid='%s'",  
                    cle_uuid,
                    event_name,
                    forward,
                    call_type,
                    buy_cost,
                    sell_cost,
                    cust_id,
                    caller_read_codec,
                    caller_write_codec,
                    core_uuid);
             execute_sql(globals.odbc_dsn, query, globals.mutex);
             switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " Bridge  event query:%s  \n",query);
             switch_safe_free(query);
      }
    

//feedback_cdr

     if (!strcasecmp(call_type,"call_queue") || !strcasecmp(application,"call_sticky_agent") || !strcasecmp(application,"call_cg_sticky_agent") || !strcasecmp(application,"call_group")){
               
      
            query =  switch_mprintf("insert into pbx_feedback(uuid,src,dst,customer_id,forward,ref_id )values('%s','%s','%s','%s','%s','%s')",
                core_uuid,
                src,
                dest,
                cust_id,
                forward,
                ref_id
            );
             execute_sql(globals.odbc_dsn, query, globals.mutex);
             switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, " Bridge  event feedback query:%s  \n",query);
             switch_safe_free(query);
    }

    
    }
 




    if (!strcasecmp(event_name, "CHANNEL_HANGUP")) {
    const char *hangup_time_str = switch_event_get_header_nil(event, "Event-Date-Timestamp");
    const char *ans_time_str = switch_event_get_header_nil(event, "Caller-Channel-Answered-Time");
    const char *created_time_str = switch_event_get_header_nil(event, "Caller-Channel-Created-Time");
    const char *hangup_cause = switch_event_get_header_nil(event, "Hangup-Cause");
    const char *dialstatus = switch_event_get_header_nil(event, "variable_DIALSTATUS");

   
    char *bridge_time_str = NULL;
    char *session_time_str = NULL;
    
    long long int hangup_time, ans_time, created_time;
   

    time_t current_time;
    struct tm *time_info;
    char time_string[50]; // Adjust size as needed
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);




     if (!query) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Failed to allocate memory for query\n");
        return;
    }
    if (hangup_time_str && ans_time_str &&
        sscanf(hangup_time_str, "%lld", &hangup_time) == 1 &&
        sscanf(ans_time_str, "%lld", &ans_time) == 1 &&
        ans_time > 0) {
        long long int bridge_time_int = (hangup_time - ans_time) / 1000000;
        bridge_time_str = (char *)malloc(20 * sizeof(char));
        if (!bridge_time_str) {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Failed to allocate memory for bridge_time_str\n");
            free(query);
            return;
        }
        snprintf(bridge_time_str, 20, "%lld", bridge_time_int);
    }

    if (hangup_time_str && created_time_str &&
        sscanf(hangup_time_str, "%lld", &hangup_time) == 1 &&
        sscanf(created_time_str, "%lld", &created_time) == 1) {
        long long int session_time_int = (hangup_time - created_time) / 1000000;
        session_time_str = (char *)malloc(20 * sizeof(char));
        if (!session_time_str) {
            switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Failed to allocate memory for session_time_str\n");
            free(bridge_time_str);
            free(query);
            return;
        }
        snprintf(session_time_str, 20, "%lld", session_time_int);
    }

    snprintf(query, 500, "update pbx_realtime_cdr set call_disposition = '%s', bridge_time = '%s', session_time = '%s', sip_endpoint_disposition = '%s', current_status = '%s', end_time = '%s' where uuid = '%s'",
             hangup_cause ? hangup_cause : "", bridge_time_str ? bridge_time_str : "0", session_time_str ? session_time_str : "0", dialstatus, event_name, time_string, uuid);

    execute_sql(globals.odbc_dsn, query, globals.mutex);
    //switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, " hangup  event : %s\n", query);

    // Free allocated memory
    free(bridge_time_str);
    free(session_time_str);
    free(query);
}
 

  if (!strcasecmp(event_name, "CHANNEL_DESTROY")) 
  {


    
    const char *clr_signallingip = switch_event_get_header_nil(event, "variable_sip_h_X-userip");
    const char *clr_media_ip = switch_event_get_header_nil(event, "variable_sip_h_X-userMediaIP");
    const char *server_ip = switch_event_get_header_nil(event, "variable_sip_local_network_addr");
    const char *user_agent = switch_event_get_header_nil(event, "variable_sip_user_agent");
    const char *calleeSignalingIP = switch_event_get_header_nil(event, "variable_sip_rh_X-calleeSignalingIP");
    const char *calleeMediaIP = switch_event_get_header_nil(event, "variable_sip_rh_X-calleeMediaIP");
    const char *dtmf = switch_event_get_header_nil(event, "variable_dtmf");
    //const char * geo_loc =switch_event_get_header_nil(event, "variable_geo_location");
    char *cle_lat_long = NULL;
    
   

    char *clr_lat_long = NULL;
    char *query = (char *)malloc(1000 * sizeof(char)); // Adjust size as needed
    if (!query) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Failed to allocate memory for query\n");
        return;
    }

   /*   if (geo_loc && strcmp(geo_loc, "1") == 0) {
        if (clr_signallingip) {
            clr_lat_long = get_loc(clr_signallingip);
        } else {
            clr_signallingip = switch_event_get_header_nil(event, "variable_sip_via_host");
            clr_media_ip = switch_event_get_header_nil(event, "variable_remote_media_ip");
        }
    }  */
    

    if (atoi(switch_event_get_header_nil(event, "variable_rtp_audio_in_mos")))
    {
      
    
    
    snprintf(query, 1000, "update pbx_realtime_cdr set clr_mos = '%s', clr_jitter_min_variance = '%s', clr_jitter_max_variance = '%s', customer_id = '%s', id_callplan = '%s', minute_plan_id = '%s', direct_gateway = '%s', clr_jitter_loss_rate = '%s', clr_jitter_burst_rate = '%s', clr_mean_interval = '%s', clr_quality_percentage = '%s', clr_read_codec = '%s', clr_write_codec = '%s', clr_dtmf_type = '%s', clr_local_media_ip = '%s', clr_media_ip = '%s', clr_user_agent = '%s', dtmf = '%s', sip_current_application = '%s', id_trunk = '%s', clr_remote_media_ip = '%s', clr_signalling_ip = '%s', clr_remote_media_port = '%s', current_status = 'CHANNEL_HANGUP', clr_lat_long = '%s' where uuid = '%s'",
             switch_event_get_header_nil(event, "variable_rtp_audio_in_mos") ,
             switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_min_variance"),
             switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_max_variance"),
             cust_id,
             id_callplan,
             minute_plan_id,
             gtwy,
             switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_loss_rate"),
             switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_burst_rate"),
             switch_event_get_header_nil(event, "variable_rtp_audio_in_mean_interval"),
             switch_event_get_header_nil(event, "variable_rtp_audio_in_quality_percentage"),
             switch_event_get_header_nil(event, "variable_original_read_codec"),
             switch_event_get_header_nil(event, "variable_write_codec"),
             switch_event_get_header_nil(event, "variable_dtmf_type"),
             clr_media_ip,
             clr_media_ip,
             user_agent,
             dtmf,
             call_type,
             trunk_id,
             clr_signallingip,
             clr_signallingip,
             switch_event_get_header_nil(event, "variable_local_media_port"),
             clr_lat_long ? clr_lat_long : "",
             uuid);
    
     execute_sql(globals.odbc_dsn, query, globals.mutex);   
    // switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " query %s\n",query);
  
 /*   if (calleeSignalingIP) {
        
        if (strcmp(calleeSignalingIP, "None") != 0) {
            cle_lat_long = get_loc(calleeSignalingIP);
        } else {
            calleeSignalingIP = switch_event_get_header_nil(event, "variable_sip_reply_host");
            calleeMediaIP = switch_event_get_header_nil(event, "variable_remote_media_ip");
        }
      } */

     switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_DEBUG, "%s  %s",(switch_event_get_header_nil(event, "variable_rtp_audio_in_mos")), switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_min_variance"));
      
        snprintf(query, 1000, "update pbx_realtime_cdr set cle_lat_long = '%s', cle_mos = '%s', cle_jitter_min_variance = '%s', cle_jitter_max_variance = '%s', customer_id = '%s', id_callplan = '%s', minute_plan_id = '%s', direct_gateway = '%s', cle_jitter_loss_rate = '%s', cle_jitter_burst_rate = '%s', cle_mean_interval = '%s', cle_quality_percentage = '%s', cle_read_codec = '%s', cle_write_codec = '%s', cle_dtmf_type = '%s', cle_local_media_ip = '%s', cle_media_ip = '%s', cle_user_agent = '%s', dtmf = '%s', sip_current_application = '%s', id_trunk = '%s', cle_remote_media_ip = '%s', cle_signalling_ip = '%s', cle_remote_media_port = '%s', server_ip = '%s' where cle_uuid = '%s'",
                 cle_lat_long ? cle_lat_long : "",
                 switch_event_get_header_nil(event, "variable_rtp_audio_in_mos"),
                 switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_min_variance"),
                 switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_max_variance"),
                 cust_id,
                 id_callplan,
                 minute_plan_id,
                 gtwy,
                 switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_loss_rate"),
                 switch_event_get_header_nil(event, "variable_rtp_audio_in_jitter_burst_rate"),
                 switch_event_get_header_nil(event, "variable_rtp_audio_in_mean_interval"),
                 switch_event_get_header_nil(event, "variable_rtp_audio_in_quality_percentage"),
                 switch_event_get_header_nil(event, "variable_original_read_codec"),
                 switch_event_get_header_nil(event, "variable_write_codec"),
                 switch_event_get_header_nil(event, "variable_dtmf_type"),
                 calleeMediaIP,
                 calleeMediaIP,
                 user_agent,
                 dtmf,
                 call_type,
                 trunk_id,
                 calleeSignalingIP,
                 calleeSignalingIP,
                 switch_event_get_header_nil(event, "variable_remote_media_port"),
                 server_ip,
                 uuid);

        execute_sql(globals.odbc_dsn, query, globals.mutex);     
    }
    

    free(query);
}

  
 if (!strcasecmp(event_name, "CHANNEL_HANGUP_COMPLETE")) 
 {
  

    const char *call_type = switch_event_get_header_nil(event, "variable_call_type");
   // const char *cc_switch = switch_event_get_header_nil(event, "variable_sip_h_X-switch");
    //const char *sip_ip = switch_event_get_header_nil(event, "variable_sip_network_ip");
    const char *dialstatus_check = switch_event_get_header_nil(event, "variable_hangup_cause");
    const char *forward = switch_event_get_header_nil(event, "Caller-Callee-ID-Number");
    char  *record_value = switch_event_get_header_nil(event, "variable_set_recording");
    const char *record_sec = switch_event_get_header_nil(event, "variable_record_seconds");
    const char *record_file = switch_event_get_header_nil(event,"variable_recording_file");
    const char *term_cause = switch_event_get_header_nil(event, "var701iable_custom_hangup");
    const char *failed_cause = switch_event_get_header_nil(event, "variable_originate_failed_cause");
    //const char *is_voicemail = switch_event_get_header_nil(event, "variable_voicemail_file_path");
    const char *rec_num = switch_event_get_header_nil(event, "variable_rec_num");
    const char *rec_callee = switch_event_get_header_nil(event, "variable_rec_callee");
    const char *sip_invite_failure_status = switch_event_get_header_nil(event, "variable_sip_invite_failure_status");
    const char *sip_term_status = switch_event_get_header_nil(event, "variable_sip_term_status");
    const char *proto_specific_hangup_cause = switch_event_get_header_nil(event, "variable_last_bridge_proto_specific_hangup_cause");
    const char *hangup_dis = switch_event_get_header_nil(event, "variable_sip_hangup_disposition");
    const char *num = switch_event_get_header_nil(event, "Caller-Username");
    const char *callee = switch_event_get_header_nil(event, "Caller-Destination-Number");
    const char *customer_id = switch_event_get_header_nil(event, "variable_cust_id");
    //const char *date = switch_event_get_header_nil(event, "Event-Date-Local");
    const char *rec_type = switch_event_get_header_nil(event, "variable_rec_type");
    const char *patch_forward = switch_event_get_header_nil(event, "variable_patch_forward");
    const char *participant_id = switch_event_get_header_nil(event, "variable_participant_id");
    const char *job_uuid = switch_event_get_header_nil(event, "variable_Job_uuid");
    const char *conf_id = switch_event_get_header_nil(event, "variable_conf_id");
    const char *last_arg = switch_event_get_header_nil(event, "variable_last_arg");
    char is_record;
    char query[QUERY_MAX_LENGTH]; 



// switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, " term_cause  %d\n",atoi(term_cause));

  if (atoi(term_cause) == 0) {
    term_cause = sip_term_status;
}
if (atoi(term_cause) == 0) {
    term_cause = sip_invite_failure_status;
}
if (atoi(term_cause) == 0) {
    if (strncmp(proto_specific_hangup_cause, "sip:", 4) == 0) {
        term_cause = proto_specific_hangup_cause + 4;
    }
}

if (atoi(term_cause) == 0) {
    if (strcmp(failed_cause, "NO_ANSWER") == 0) {
        term_cause = "480";
    } else if (strcmp(dialstatus_check, "NORMAL_CLEARING") == 0) {
        term_cause = "200";
    } else if (strcmp(dialstatus_check, "ALLOTTED_TIMEOUT") == 0) {
        term_cause = "602";
    } else if (strcmp(dialstatus_check, "USER_BUSY") == 0) {
        term_cause = "486";
    } else if (strcmp(dialstatus_check, "UNKNOWN") == 0 && strcmp(last_arg, "+0 1006") == 0) {
        term_cause = "1006";
    } else {
        term_cause = "608";
    }
}

// Set hangup_dis based on variable_sip_hangup_disposition
if (strcmp(hangup_dis, "recv_cancel") == 0) {
    hangup_dis = "recv_bye";
} else if (strcmp(hangup_dis, "send_refuse") == 0 && strcmp(failed_cause, "SUCCESS") == 0) {
    hangup_dis = "recv_cancel";
} else if (strcmp(hangup_dis, "send_refuse") == 0 && strcmp(failed_cause, "NOANSWER") != 0) {
    hangup_dis = "recv_cancel";
} else if (strcmp(hangup_dis, "recv_refuse") == 0) {
    hangup_dis = "recv_cancel";
}

// Set forward if patch_forward is not "None"


if (atoi(patch_forward) != 0) {
    forward = patch_forward;
}

snprintf(query,200, "update pbx_realtime_cdr set terminatecause='%s', hangup_disposition='%s', is_forward='0', forward ='%s' where uuid='%s'", term_cause, hangup_dis, forward, uuid);
execute_sql(globals.odbc_dsn, query, globals.mutex);
//switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " query %s\n",query);



// Set is_record to '1' if call_type is "call_tc"


if (record_value != NULL) {
    
    is_record = *record_value;
} else {
   
    is_record = '0'; 
}

if (strcmp(call_type, "call_tc") == 0) {
    is_record = '1';
}


if (is_record =='1' && strcmp(call_type, "call_voicemail") != 0 &&  record_sec && atoi(record_sec) != 0 ) {
    if (strcmp(call_type, "call_queue") == 0 || strcmp(call_type, "call_tc") == 0) {
        rec_callee = callee;
        rec_num = num;
        
        record_file = strrchr(switch_event_get_header_nil(event, "variable_cc_record_filename"),'/');
        if (strcmp(call_type, "call_tc") == 0) {
            rec_type = "Tele Consultancy";
        } else {
            rec_type = "Call Queue";
        }
    } else if (strcmp(call_type, "call_obd") == 0 || strcmp(call_type, "Outbound Conference") == 0) {

       
        record_file = strrchr(switch_event_get_header_nil(event,"variable_execute_on_originate"), '/');
    } else {
        record_file = strrchr(record_file, '/');
    }


   
    if (record_file) {
        snprintf(query, QUERY_MAX_LENGTH, "INSERT INTO `pbx_recording` (`file_name`, `customer_id`, `src`, `type`, `dest`, `status`, `uuid`) VALUES ('%s', '%s', '%s', '%s', '%s', '1', '%s')", record_file++, customer_id, rec_num, rec_type, rec_callee, uuid);
        execute_sql(globals.odbc_dsn, query, globals.mutex);
        //switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, " recording query %s\n",query);

    }
}

if ((strcmp(call_type, "click2call") == 0 || strcmp(call_type, "patch_api") == 0) && record_sec && atoi(record_sec) != 0 ) {

    if (atoi(switch_event_get_header_nil(event, "variable_billsec")) > 0) {

        char *rec = switch_event_get_header_nil(event,"variable_execute_on_originate");
        char *record = strrchr(rec, '/');
        callee = switch_event_get_header_nil(event, "Caller-Callee-ID-Number");
        num = switch_event_get_header_nil(event, "Other-Leg-Destination-Number");
        snprintf(query, QUERY_MAX_LENGTH, "INSERT INTO `pbx_recording` (`file_name`, `customer_id`, `src`, `type`, `dest`, `status`, `uuid`) VALUES ('%s', '%s', '%s', '%s', '%s', '1', '%s')", record++, customer_id, num, call_type, callee, uuid);
        execute_sql(globals.odbc_dsn, query, globals.mutex);
    }
}

/* if (strcmp(call_type, "call_voicemail") == 0 && strcmp(is_voicemail, "None") != 0) {

    char **vm_callback =   execute_sql(globals.odbc_dsn, query, globals.mutex);
    char **msg = strtok(strdup(is_voicemail), "/");
    callee = switch_event_get_header_nil(event,"variable_voicemail_account");
    snprintf(query, QUERY_MAX_LENGTH, "SELECT otherEmail, voicemailToEmail, delVoicemailAfterEmail, deliverVoicemailTo FROM pbx_voicemail JOIN extension_master ON pbx_voicemail.extension_id = extension_master.id WHERE extension_master.ext_number = '%s'", callee);
    if (vm_callback == NULL || (vm_callback[0][2]) == 0) {
        snprintf(query, QUERY_MAX_LENGTH, "INSERT INTO `pbx_voicemail_recording` (`file_name`, `customer_id`, `src`, `dest`, `status`, `created_at`) VALUES ('%s', '%s', '%s', '%s', '1', '%s')", msg[-1], customer_id, num, callee, date);
        execute_sql(globals.odbc_dsn, query, globals.mutex);
    }
}
 */
if (strcmp(call_type, "call_obd") == 0) {
   
    char *obd_id = switch_event_get_header_nil(event,"variable_obd_id");
    char *dtmf = switch_event_get_header_nil(event,"variable_dtmf_store");
    snprintf(query, QUERY_MAX_LENGTH, "call switch_obd_instance_id_hangup('%s', '%s', '%s', '%s', '%s')", obd_id, participant_id, job_uuid, term_cause, dtmf);
    execute_sql(globals.odbc_dsn, query, globals.mutex);
}

if (strcmp(call_type, "Outbound Conference") == 0 && conf_id) {
    snprintf(query, QUERY_MAX_LENGTH, "call switch_confererce_instance_id_hangup('%s', '%s', '%s', '%s')", conf_id, participant_id, job_uuid, term_cause);
      execute_sql(globals.odbc_dsn, query, globals.mutex);
}

if (strcmp(call_type, "call_broadcast") == 0) {
    char *bc_mapped_id =switch_event_get_header_nil(event,"variable_bc_mapped_id");
    snprintf(query, QUERY_MAX_LENGTH, "call switch_bc_instance_id_hangup('%s', '%s', '%s', '%s')", ref_id, bc_mapped_id, job_uuid, term_cause);
      execute_sql(globals.odbc_dsn, query, globals.mutex);
}


 }
 
 }
