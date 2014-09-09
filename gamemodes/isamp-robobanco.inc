 #if defined _isamp_robobanco_included
	#endinput
#endif
#define _isamp_robobanco_included

//==========================SISTEMA DE ROBO AL BANCO============================

//================================CONSTANTES====================================

#define MAX_GROUP_MEMBERS 		3
#define DINERO_BOVEDA     		200000

#define MIN_DUTYCOPS_NEEDED  	3
#define MIN_THIEFS_NEEDED 		4

#define ROBBERY_GROUP_NONE      -1

#define POS_BANK_DOOR_X         2305.53
#define POS_BANK_DOOR_Y         -15.85
#define POS_BANK_DOOR_Z         26.74

#define STAGE_COMPLETED         MIN_THIEFS_NEEDED

#define STAGE_INICIO_ROBO       0
#define STAGE_REDUCIR_ENTORNO   1
#define STAGE_DETONAR_PUERTA    2
#define STAGE_DETONAR_BOVEDA    3
#define STAGE_TOMAR_DINERO      4

//==============================DATA STORAGE====================================

new
	bool:HoldingMoney[MAX_PLAYERS],
	HoldingMoneyTimer[MAX_PLAYERS];

enum RobberyGroupInfo {
	bool:ActiveLeadership,
	MembersID[MAX_GROUP_MEMBERS],
	RobberyStage,
	RobberyStageProgress
};

new RobberyGroup[MAX_PLAYERS][RobberyGroupInfo],
	RobberyGroupNumber[MAX_PLAYERS],
	PlayerRobberyStage[MAX_PLAYERS],
	GroupOffer[MAX_PLAYERS];

//===============================FUNCIONES======================================

forward ResetRobberyGroup(playerid);
forward bool:AddRobberyGroupMember(leaderid, playerid); // Agrega a "playerid" como miembro del grupo de robo del lider "leaderid". Si tuvo �xito, devuelve true. Si no hay espacio, false.
forward CountRobberyGroupMembers(leaderGroup); // Cuenta la cantidad de miembros conectados que hay en un grupo de robo (incluye al lider).
forward CancelGroupOffer(playerid, reason);
forward PrintRobberyGroupForPlayer(leaderid, playerid); // Imprime los miembros del grupo de "liderid" al jugador "playerid". Cabe mencionar que en este sistema, la id del lider coincide con la del grupo.
forward GetRobberyGroupStage(group); // Devuelve la etapa en la que se encuentra el grupo de robo.

forward TimeReducirEntorno(playerid);
forward TimeDetonacionPuerta(playerid);
forward TimeDetonacionBoveda(playerid);
forward TimeDineroBoveda(playerid);
forward TimeHoldMoney(playerid);

forward IsPlayerRobberyGroupLider(playerid); // Devuelve true si el jugador es lider de un grupo, false en caso contrario.

//======================IMPLEMENTACION DE FUNCIONES=============================

stock GetRobberyGroupStage(group)
{
	return RobberyGroup[group][RobberyStage];
}

stock IsPlayerRobberyGroupLider(playerid)
{
	if(RobberyGroup[playerid][ActiveLeadership] == true && RobberyGroupNumber[playerid] == playerid)
	    return true;
	else
	    return false;
}

stock ResetRobberyGroup(playerid)
{
	RobberyGroup[playerid][ActiveLeadership] = false;
	for(new i = 0; i < MAX_GROUP_MEMBERS; i++)
	{
	    if(RobberyGroup[playerid][MembersID][i] != INVALID_PLAYER_ID)
	    {
	    	RobberyGroupNumber[RobberyGroup[playerid][MembersID][i]] = ROBBERY_GROUP_NONE;
	    	PlayerRobberyStage[RobberyGroup[playerid][MembersID][i]] = 0;
	    	RobberyGroup[playerid][MembersID][i] = INVALID_PLAYER_ID;
		}
	}
	RobberyGroup[playerid][RobberyStage] = 0;
	RobberyGroup[playerid][RobberyStageProgress] = 0;
    RobberyGroupNumber[playerid] = ROBBERY_GROUP_NONE;
    GroupOffer[playerid] = INVALID_PLAYER_ID;
	HoldingMoneyTimer[playerid] = 0;
	HoldingMoney[playerid] = false;
}

stock bool:AddRobberyGroupMember(leaderid, playerid)
{
	for(new i = 0; i < MAX_GROUP_MEMBERS; i++)
	{
	    if(RobberyGroup[leaderid][MembersID][i] == INVALID_PLAYER_ID)
	    {
	        RobberyGroup[leaderid][MembersID][i] = playerid;
	        RobberyGroupNumber[playerid] = leaderid;
	        return true;
		}
	}
	return false;
}

stock CountRobberyGroupMembers(leaderGroup)
{
	new aux;
	
	if(IsPlayerConnected(leaderGroup))
	    aux++;
	for(new i = 0; i < MAX_GROUP_MEMBERS; i++)
	{
	    if(IsPlayerConnected(RobberyGroup[leaderGroup][MembersID][i]))
	        aux ++;
	}
	return aux;
}

public CancelGroupOffer(playerid, reason)
{
	if(reason == 1)
	{
		SendClientMessage(playerid, COLOR_LIGHTBLUE, "La invitaci�n ha sido cancelada ya que no has respondido en 15 segundos.");
		SendClientMessage(GroupOffer[playerid], COLOR_LIGHTBLUE, "La invitaci�n ha sido cancelada ya que el sujeto no ha respondido en 15 segundos.");
	} else
		if(reason == 0)
		{
	    	SendClientMessage(playerid, COLOR_LIGHTBLUE, "Has rechazado la invitaci�n.");
			SendFMessage(GroupOffer[playerid], COLOR_LIGHTBLUE, "%s ha rechazado la invitaci�n.", GetPlayerNameEx(playerid));
		}
	GroupOffer[playerid] = INVALID_PLAYER_ID;
	return 1;
}

stock PrintRobberyGroupForPlayer(leaderid, playerid)
{
	new memberid;
	
	SendClientMessage(playerid, COLOR_LIGHTYELLOW2, "==========[Grupo de robo al Banco Central de Malos Aires]==========");
	SendFMessage(playerid, COLOR_WHITE, " Lider: %s (ID %d)", GetPlayerNameEx(leaderid), leaderid);
	for(new i = 0; i < MAX_GROUP_MEMBERS; i++)
	{
	    memberid = RobberyGroup[leaderid][MembersID][i];
	    if(memberid != INVALID_PLAYER_ID)
	    {
	        if(IsPlayerConnected(memberid))
 				SendFMessage(playerid, COLOR_WHITE, " %d - Miembro: %s (ID %d)" , i, GetPlayerNameEx(memberid), memberid);
			else
			    SendFMessage(playerid, COLOR_WHITE, " %d - Miembro: %s (Desconectado)" , i, GetPlayerNameEx(memberid));
		}
		else
		    SendFMessage(playerid, COLOR_WHITE, " %d - Miembro: Ninguno" , i);
	}
	return 1;
}

//================================COMANDOS======================================

CMD:grupoayuda(playerid, params[])
{
	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");

	SendClientMessage(playerid, COLOR_WHITE, "[Comandos]: /grupocrear /grupoeliminar /grupoinfo /grupoinvitar /grupoaceptar /grupocancelar /grupoexpulsar");
	SendClientMessage(playerid, COLOR_WHITE, "[Info]: Para asaltar un banco, deber�s crear un grupo e invitar a otros delincuentes para que se unan a este.");
	SendClientMessage(playerid, COLOR_WHITE, "[Comandos]: /gruporobarbanco /gruporeducirentorno /grupodetonarpuerta /grupodetonarboveda /gruporobardinero");
	return 1;
}

CMD:grupoexpulsar(playerid, params[])
{
	new
		memberslot,
		memberid;

	if(!IsPlayerRobberyGroupLider(playerid))
    	return SendClientMessage(playerid, COLOR_YELLOW2, "No eres lider de ning�n grupo de robo.");
	if(sscanf(params, "i", memberslot))
		return SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /grupoexpulsar  [slot miembro]");
	if(memberslot < 0 || memberslot >= MAX_GROUP_MEMBERS)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Slot de miembro inv�lido. Puedes mirarlo en /grupoinfo.");
	memberid = RobberyGroup[playerid][MembersID][memberslot];
	if(memberid == INVALID_PLAYER_ID)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No figura ning�n miembro en ese slot. Puedes mirarlo en /grupoinfo.");
	
	SendFMessage(playerid, COLOR_WHITE, "Has expulsado a %s del grupo de robo.", GetPlayerNameEx(memberid));
	SendFMessage(memberid, COLOR_WHITE, "Has sido expulsado por %s del grupo de robo.", GetPlayerNameEx(playerid));
	RobberyGroupNumber[memberid] = ROBBERY_GROUP_NONE;
	PlayerRobberyStage[memberid] = 0;
	RobberyGroup[playerid][MembersID][memberslot] = INVALID_PLAYER_ID;
	return 1;
}

CMD:grupoeliminar(playerid, params[])
{
	if(!IsPlayerRobberyGroupLider(playerid))
    	return SendClientMessage(playerid, COLOR_YELLOW2, "No eres lider de ning�n grupo de robo.");
    	
	ResetRobberyGroup(playerid);
	SendClientMessage(playerid, COLOR_WHITE, "Has eliminado el grupo de robo del cual eras lider. Mas info en /grupoayuda.");
	return 1;
}

CMD:grupocrear(playerid, params[])
{ 
	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 8)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Necesitas m�s experiencia para utilizar este comando.");
	if(RobberyGroupNumber[playerid] != ROBBERY_GROUP_NONE)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Ya te encuentras en un grupo de robo.");
	
    RobberyGroup[playerid][ActiveLeadership] = true;
    RobberyGroupNumber[playerid] = playerid; // La id del grupo es la id del leader.
	SendClientMessage(playerid, COLOR_WHITE, "Has creado un grupo de robo para asaltar el Banco Central de Malos Aires. Mas info en /grupoayuda.");
  	return 1;
}

CMD:grupoinfo(playerid, params[])
{
	new
		group = RobberyGroupNumber[playerid],
		str[128];
		
	if(group != ROBBERY_GROUP_NONE)
	{
	    PrintRobberyGroupForPlayer(group, playerid);
	    switch(RobberyGroup[group][RobberyStage])
	    {
	        case STAGE_INICIO_ROBO: format(str, sizeof(str), "Inicio de robo");
	        case STAGE_REDUCIR_ENTORNO: format(str, sizeof(str), "Reducci�n de empleados y rehenes");
	        case STAGE_DETONAR_PUERTA: format(str, sizeof(str), "Detonaci�n de la puerta principal");
	        case STAGE_DETONAR_BOVEDA: format(str, sizeof(str), "Detonaci�n de la b�veda");
	        case STAGE_TOMAR_DINERO: format(str, sizeof(str), "Extracci�n de dinero de la b�veda");
		}
	    SendFMessage(playerid, COLOR_WHITE, "Etapa: %s - Progreso: %d de %d", str, RobberyGroup[group][RobberyStageProgress], MIN_THIEFS_NEEDED);
	    SendClientMessage(playerid, COLOR_LIGHTYELLOW2, "============================================================");
	}
	return 1;
}

CMD:grupoinvitar(playerid, params[])
{
	new targetid;

	if(IsPlayerRobberyGroupLider(playerid))
	{
		if(sscanf(params, "u", targetid))
	    	return SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /grupoinvitar  [ID/Jugador]");
  		if(targetid == INVALID_PLAYER_ID || targetid == playerid || !IsPlayerConnected(targetid))
	        return SendClientMessage(playerid, COLOR_YELLOW2, "Jugador invalido.");
		if(PlayerInfo[targetid][pJob] != JOB_FELON)
	        return SendClientMessage(playerid, COLOR_YELLOW2, "El sujeto tiene que tener el trabajo de delincuente.");
	 	if(ThiefJobInfo[targetid][pFelonLevel] < 7)
	        return SendClientMessage(playerid, COLOR_YELLOW2, "El sujeto necesita m�s experiencia como delincuente para formar un grupo");
		if(RobberyGroupNumber[targetid] != ROBBERY_GROUP_NONE)
	        return SendClientMessage(playerid, COLOR_YELLOW2, "Este sujeto ya pertenece a un grupo de robo.");

	 	GroupOffer[targetid] = playerid;
	 	SendFMessage(playerid, COLOR_LIGHTBLUE, "Invitaste a %s a formar parte de tu grupo de robo.", GetPlayerNameEx(targetid));
	 	SendFMessage(targetid, COLOR_LIGHTBLUE, "%s te invit� a su grupo de robo. Para aceptar usa /grupoaceptar, de lo contrario usa /grupocancelar.", GetPlayerNameEx(playerid));
	 	SetPVarInt(targetid, "CancelGroupOffer", SetTimerEx("CancelGroupOffer", 15 * 1000, false, "ii", targetid, 1));
	}
	return 1;
}

CMD:grupoaceptar(playerid, params[])
{
	new groupLeader = GroupOffer[playerid];

	if(groupLeader == INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_YELLOW2, "Nadie te est� invitando a un grupo.");
	if(RobberyGroupNumber[playerid] != ROBBERY_GROUP_NONE)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Ya perteneces a otro grupo de robo.");
    if(!IsPlayerConnected(groupLeader) || !ProxDetectorS(4.0, playerid, groupLeader))
    {
		KillTimer(GetPVarInt(playerid, "CancelGroupOffer"));
		CancelGroupOffer(playerid, 0);
		return SendClientMessage(playerid, COLOR_YELLOW2, "Jugador inv�lido o se encuentra muy lejos.");
	}
	if(!AddRobberyGroupMember(groupLeader, playerid))
    {
		KillTimer(GetPVarInt(playerid, "CancelGroupOffer"));
		CancelGroupOffer(playerid, 0);
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�No hay mas espacio para miembros en ese grupo!");
	}

	SendFMessage(playerid, COLOR_LIGHTBLUE, "�Excelente, ahora estas en el grupo de robo liderado por %s! Usa /grupoinfo para mas informaci�n.", GetPlayerNameEx(groupLeader));
  	SendFMessage(groupLeader, COLOR_LIGHTBLUE, "�Excelente, %s acept� y ahora forma parte de tu grupo de robo.", GetPlayerNameEx(playerid));
  	KillTimer(GetPVarInt(playerid, "CancelGroupOffer"));
	CancelGroupOffer(playerid, 2);
	return 1;
}

CMD:grupocancelar(playerid, params[])
{
	if(GroupOffer[playerid] == INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_YELLOW2, "Nadie te est� invitando a un grupo.");
	        
	KillTimer(GetPVarInt(playerid, "CancelGroupOffer"));
	CancelGroupOffer(playerid, 0);
    return 1;
}

//================================COMANDOS======================================

CMD:gruporobarbanco(playerid, params[])
{
	if(!IsPlayerRobberyGroupLider(playerid))
    	return SendClientMessage(playerid, COLOR_YELLOW2, "Debes ser el lider de un grupo de robo para utilizar este comando.");
	if(!IsPlayerInRangeOfPoint(playerid, 10.0, POS_BANK_DOOR_X, POS_BANK_DOOR_Y, POS_BANK_DOOR_Z))
	    return SendClientMessage(playerid, COLOR_YELLOW2, "El grupo completo debe estar en el interior del Banco Central de Malos Aires, cercanos a la puerta.");
	for(new i = 0; i < MAX_GROUP_MEMBERS; i++)
 	{
		if(IsPlayerConnected(RobberyGroup[playerid][MembersID][i]))
		{
			if(!IsPlayerInRangeOfPoint(RobberyGroup[playerid][MembersID][i], 10.0, POS_BANK_DOOR_X, POS_BANK_DOOR_Y, POS_BANK_DOOR_Z))
	    		return SendClientMessage(playerid, COLOR_YELLOW2, "El grupo completo debe estar en el interior del Banco Central de Malos Aires, cercanos a la puerta.");
		}
	}
 	if(CountCopsOnDuty() < MIN_DUTYCOPS_NEEDED)
 		return SendClientMessage(playerid, COLOR_YELLOW2, "Deben haber minimo tres policias en servicio conectados.");
	if(CountRobberyGroupMembers(playerid) < MIN_THIEFS_NEEDED)
 		return SendClientMessage(playerid, COLOR_YELLOW2, "Recuerda que en tu grupo de robo deben ser cuatro, de lo contrario no podras robar el banco.");
	if(GetRobberyGroupStage(playerid) != STAGE_INICIO_ROBO)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Tu grupo ya se encuentra en una etapa mas avanzada del robo.");

 	RobberyGroup[playerid][RobberyStage] = STAGE_REDUCIR_ENTORNO;
 	RobberyGroup[playerid][RobberyStageProgress] = 0;
 	PoliceBankRobberyAlarm();
 	SendClientMessage(playerid, COLOR_LIGHTBLUE, "Comienzas a robar el banco de malos aires. Tienes que hacerlo rapido pues la alarma silenciosa avis� a la polic�a.");
 	SendClientMessage(playerid, COLOR_LIGHTBLUE, "El robo consiste en etapas: reduce a los rehenes y detona las b�vedas para llevate el dinero. Mas info en /grupoayuda.");
 	return 1;
}

forward PoliceBankRobberyAlarm();
public PoliceBankRobberyAlarm()
{
	new string[128];

	format(string, sizeof(string), "[Dpto. de polic�a]: Ha sonado la alarma del Banco Central de Malos Aires. �Lo est�n robando!");
 	foreach(new i : Player)
 	{
		if(isPlayerCopOnDuty(i))
		{
		    SendClientMessage(i, COLOR_PMA, "================================[URGENTE]====================================");
			SendClientMessage(i, COLOR_PMA, string);
		}
 	}
}


CMD:gruporeducirentorno(playerid, params[])
{
	if(RobberyGroupNumber[playerid] == ROBBERY_GROUP_NONE)
    	return 1;
	if(PlayerRobberyStage[playerid] != STAGE_INICIO_ROBO)
	    return 1;
	if(GetRobberyGroupStage(RobberyGroupNumber[playerid]) != STAGE_REDUCIR_ENTORNO)
	    return SendClientMessage(playerid,COLOR_YELLOW2, "Tu grupo se encuentra en otra etapa del robo.");
  	if(CountRobberyGroupMembers(RobberyGroupNumber[playerid]) < MIN_THIEFS_NEEDED)
 		return SendClientMessage(playerid, COLOR_YELLOW2, "Recuerda que en tu grupo de robo deben ser cuatro, de lo contrario no podras robar el banco.");

	PlayerActionMessage(playerid, 15.0, "toma su arma y apunta a las personas que estan en el lugar, haciendo que estas se tiren al suelo.");
	PlayerRobberyStage[playerid] = STAGE_REDUCIR_ENTORNO;
	SetPVarInt(playerid, "disabled", DISABLE_STEALING);
 	SetTimerEx("TimeReducirEntorno", 20 * 1000, false, "i", playerid);
 	return 1;
}

public TimeReducirEntorno(playerid)
{
	new group = RobberyGroupNumber[playerid];
	
	RobberyGroup[group][RobberyStageProgress]++;
	if(RobberyGroup[group][RobberyStageProgress] == STAGE_COMPLETED)
	{
        RobberyGroup[group][RobberyStage] = STAGE_DETONAR_PUERTA;
		PlayerDoMessage(playerid, 15.0, "Uno de los rehenes oprimi� el boton de la alarma. Detona las puertas y toma el dinero antes que llegue la polic�a.(/detonarpuerta)");
	}
	return 1;
}

//==============================================================================

CMD:grupodetonarpuerta(playerid, params[])
{
	if(RobberyGroupNumber[playerid] == ROBBERY_GROUP_NONE)
    	return 1;
	if(PlayerRobberyStage[playerid] != STAGE_REDUCIR_ENTORNO)
	    return 1;
	if(GetRobberyGroupStage(RobberyGroupNumber[playerid]) != STAGE_DETONAR_PUERTA)
	    return SendClientMessage(playerid,COLOR_YELLOW2, "Tu grupo se encuentra en otra etapa del robo.");
	if(CountRobberyGroupMembers(RobberyGroupNumber[playerid]) < MIN_THIEFS_NEEDED)
        return SendClientMessage(playerid, COLOR_YELLOW2, "Recuerda que en tu grupo de robo deben ser cuatro, de lo contrario no podras robar el banco.");

	PlayerActionMessage(playerid, 15.0, "coloca un detonador en la puerta de entrada hacia las bovedas y se aleja esperando la explosi�n");
	PlayerRobberyStage[playerid] = STAGE_DETONAR_PUERTA;
 	SetTimerEx("TimeDetonacionPuerta", 5 * 1000, false, "i", playerid);
 	return 1;
}


public TimeDetonacionPuerta(playerid)
{
	new group = RobberyGroupNumber[playerid];
	RobberyGroup[group][RobberyStageProgress]++;
	if(RobberyGroup[group][RobberyStageProgress] == STAGE_COMPLETED)
	{
        RobberyGroup[group][RobberyStage] = STAGE_DETONAR_BOVEDA;
		PlayerDoMessage(playerid, 15.0, "La puerta principal se rompi� con la ultima detonaci�n. Ya se puede acceder a la b�veda (/detonarboveda)");
	}
	return 1;
}
				
//==============================================================================

CMD:grupodetonarboveda(playerid, params[])
{
	if(RobberyGroupNumber[playerid] == ROBBERY_GROUP_NONE)
    	return 1;
	if(PlayerRobberyStage[playerid] != STAGE_DETONAR_PUERTA)
	    return 1;
	if(GetRobberyGroupStage(RobberyGroupNumber[playerid]) != STAGE_DETONAR_BOVEDA)
	    return SendClientMessage(playerid,COLOR_YELLOW2, "Tu grupo se encuentra en otra etapa del robo.");
 	if(CountRobberyGroupMembers(RobberyGroupNumber[playerid]) < MIN_THIEFS_NEEDED)
 		return SendClientMessage(playerid, COLOR_YELLOW2, "Recuerda que en tu grupo de robo deben ser cuatro, de lo contrario no podras robar el banco.");

	PlayerActionMessage(playerid, 15.0, "coloca un detonador en la puerta de entrada hacia las bovedas y se aleja esperando la explosi�n.");
	PlayerRobberyStage[playerid] = STAGE_DETONAR_BOVEDA;
 	SetTimerEx("TimeDetonacionBoveda", 5 * 1000, false, "i", playerid);
 	return 1;
}
			
public TimeDetonacionBoveda(playerid)
{
	new group = RobberyGroupNumber[playerid];
	RobberyGroup[group][RobberyStageProgress]++;
	if(RobberyGroup[group][RobberyStageProgress] == STAGE_COMPLETED)
	{
	    RobberyGroup[group][RobberyStage] = STAGE_TOMAR_DINERO;
		PlayerDoMessage(playerid, 15.0, "La puerta de la b�veda fue detonada. Retira todo el dinero que puedas y escapa antes que llegue la polic�a. (/tomardineroboveda)");
	}
	return 1;
}
		
//==============================================================================

CMD:gruporobardinero(playerid, params[])
{	
	if(RobberyGroupNumber[playerid] == ROBBERY_GROUP_NONE)
    	return 1;
	if(PlayerRobberyStage[playerid] != STAGE_DETONAR_BOVEDA)
	    return 1;
	if(IsPlayerRobberyGroupLider(playerid))
    	return SendClientMessage(playerid, COLOR_YELLOW2, "Debes ser el lider de un grupo de robo para utilizar este comando.");
	if(GetRobberyGroupStage(RobberyGroupNumber[playerid]) != STAGE_TOMAR_DINERO)
	    return SendClientMessage(playerid,COLOR_YELLOW2, "Tu grupo se encuentra en otra etapa del robo.");
   	if(CountRobberyGroupMembers(RobberyGroupNumber[playerid]) < MIN_THIEFS_NEEDED)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Recuerda que en tu grupo de robo deben ser cuatro, de lo contrario no podras robar el banco.");

	PlayerActionMessage(playerid, 15.0, "toma una bolsa y comienza a retirar dinero de la boveda.");
	SendClientMessage(playerid, COLOR_WHITE, "Debes esperar 30 segundos mientras llenas las bolsas de dinero.");
	PlayerRobberyStage[playerid] = STAGE_TOMAR_DINERO;
	SetTimerEx("TimeDineroBoveda", 30 * 1000, false, "i", playerid);
 	return 1;
}

public TimeDineroBoveda(playerid)
{
	new
		memberid,
		groupid = RobberyGroupNumber[playerid];

	RobberyGroup[groupid][RobberyStageProgress] = STAGE_COMPLETED;
	GivePlayerCash(playerid, DINERO_BOVEDA);
	HoldingMoney[playerid] = true;
	HoldingMoneyTimer[playerid] = SetTimerEx("TimeHoldMoney", 20 * 60 * 1000, false, "i", playerid);
	SendClientMessage(playerid, COLOR_WHITE, "�Has terminado de robar el dinero, huye rapido o te atrapar�n! (OOC: Debes aguantar 20 min sin morir, deslogear o ser atrapado)");
	for(new i = 0; i < MAX_GROUP_MEMBERS; i++)
	{
	    memberid = RobberyGroup[groupid][MembersID][i];
	    SetPVarInt(memberid, "disabled", DISABLE_NONE);
		SendClientMessage(memberid, COLOR_WHITE, "�El lider del grupo tiene la guita! Cubr�lo, porque si se muere o es atrapado pierden todo el dinero robado.");
	}
	return 1;
}								

public TimeHoldMoney(playerid)
{
    HoldingMoney[playerid] = false;
} 					