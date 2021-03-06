#if defined _marp_paintball_included
	#endinput
#endif
#define _marp_paintball_included

// DEFINES
#define POS_PAINTBALL_X     	142 
#define POS_PAINTBALL_Y     	2501 
#define POS_PAINTBALL_Z     	16

#define POS_PBREDTEAMHQ_X		114
#define POS_PBREDTEAMHQ_Y		2501
#define POS_PBREDTEAMHQ_Z		16

#define POS_PBBLUETEAMHQ_X		187
#define POS_PBBLUETEAMHQ_Y		2501
#define POS_PBBLUETEAMHQ_Z		16

new PaintballPlay = 0,
    PaintballStart = 0,
    BlueTeamPlayers = 0,
	RedTeamPlayers = 0,
    BlueTeamMaxPlayers = 3,
    RedTeamMaxPlayers = 3,
    BlueTeamDeads = 0,
	RedTeamDeads = 0,
	BlueTeamSkin = 0,
	RedTeamSkin = 0;


// COMANDOS
CMD:pbcrear(playerid,params[])
{ 
	if(PaintballPlay == 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} Ya hay un evento de paintball en curso.");
	if(!IsPlayerInRangeOfPoint(playerid, 25.0, POS_PAINTBALL_X, POS_PAINTBALL_Y, POS_PAINTBALL_Z))
	    return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} Debes encontrarte en la zona donde se realizan los paintballs.");
	
	PaintballPlay = 1;
    SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Creaste un evento de Paintball. Usa /pbayuda para ver el funcionamiento del sistema.");
	return 1;
}


CMD:pbcomenzar(playerid, params[])
{
	if(PaintballPlay != 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} Antes de comenzar el juego debes crear el Paintball y configurar los equipos.");
	if(PaintballStart == 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} El juego est� en proceso, no puedes volver a iniciarlo ahora. (Usa /pbreset para terminarlo/reiniciarlo)");
	if(RedTeamSkin == 0 || BlueTeamSkin == 0)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �Uno de los equipos no tiene un skin configurado!");
	
    SendClientMessage(playerid, -1, "{878EE7}[INFO]{C8C8C8} Has comenzado el paintball.");
	PaintballStart = 1;
	foreach(new i : Player) 
	{
		if(GetPVarInt(i, "GrupoPaintball") > 0)
		{
			GameTextForPlayer(i, "YA!!!", 7000, 3);
			SendClientMessage(playerid, COLOR_RED, "{878EE7}[INFO]{C8C8C8} �El paintball comenz�! En caso de que te descalifiquen deber�s esperar a que uno de los dos equipos gane para volver a jugar.");
			SetHandItemAndParam(i, HAND_RIGHT, 0, 0);
			PhoneHand[i] = 0;
			SetPlayerHealthEx(i, 100);
			GivePlayerGun(i, 23, 100);
			if(GetPVarInt(i, "GrupoPaintball") == 1) {
			    SetPlayerSkin(i, RedTeamSkin); }
			if(GetPVarInt(i, "GrupoPaintball") == 2) {
			    SetPlayerSkin(i, BlueTeamSkin); }
		}
	}
	return 1;
}

CMD:pbmaxplayers(playerid, params[]) {
	new equipo[4], maxplayers;

	if(PaintballPlay != 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �No hay ning�n paintball creado!");
	if(PaintballStart == 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �El juego ya est� en proceso! Usa /pbreset para reiniciarlo y reconfigurar lo que necesites.");

	if(sscanf(params, "si", equipo, maxplayers)) {
		SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /pbmaxplayers [equipo: azul - rojo] [m�ximo de jugadores]");
	} else if(strcmp(equipo, "azul", true) == 0) {
		BlueTeamMaxPlayers = maxplayers;
		SendFMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Configuraste el l�mite de jugadores del equipo azul a %d.", maxplayers);
	} else if(strcmp(equipo, "rojo", true) == 0) {
		RedTeamMaxPlayers = maxplayers;
		SendFMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Configuraste el l�mite de jugadores del equipo rojo a %d.", maxplayers);
	} else {
		SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} Los posibles equipos son rojo y azul.");
	}
	return 1;
}

CMD:pbskin(playerid, params[]) {
	new equipo[4], skin;

	if(PaintballPlay != 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �No hay ning�n paintball creado!");
	if(PaintballStart == 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �El juego ya est� en proceso! Usa /pbreset para reiniciarlo y reconfigurar lo que necesites.");

	if(sscanf(params, "si", equipo, skin)) {
		SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /pbskin [equipo: azul - rojo] [skin de los jugadores]");
	} else if(strcmp(equipo, "azul", true) == 0) {
		BlueTeamSkin = skin;
		SendFMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Configuraste el skin de los jugadores del equipo azul a %d.", skin);
	} else if(strcmp(equipo, "rojo", true) == 0) {
		RedTeamSkin = skin;
		SendFMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Configuraste el skin de los jugadores del equipo rojo a %d.", skin);
	} else {
		SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} Los posibles equipos son rojo y azul.");
	}
	return 1;
}

CMD:pbinvitar(playerid, params[]) {
	new equipo[4], target;

	if(PaintballPlay != 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �No hay ning�n paintball creado!");
	if(PaintballStart == 1)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �El juego ya est� en proceso! Usa /pbreset para reiniciarlo y reconfigurar lo que necesites.");

	if(sscanf(params, "su", equipo, target)) {
		SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /pbinvitar [equipo: azul - rojo] [ID/Nombre del jugador a invitar]");
	} else if(strcmp(equipo, "azul", true) == 0) {
	    if(BlueTeamPlayers < BlueTeamMaxPlayers && target != INVALID_PLAYER_ID) {
			if(GetPVarInt(target, "GrupoPaintball") == 1)
				RedTeamPlayers -= 1;
			if(GetPVarInt(target, "GrupoPaintball") == 2)
				BlueTeamPlayers -= 1;
	        SetPVarInt(target, "GrupoPaintball", 2);
	        BlueTeamPlayers += 1;
	        SendFMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Agregaste a %s al equipo azul de Paintball.", GetPlayerNameEx(target));
			SendFMessage(target, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} El administrador %s te agreg� al equipo azul de Paintball.", GetPlayerNameEx(playerid));
		} else { SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �El equipo azul est� lleno! Pod�s modificar el l�mite de jugadores con /pbmaxplayers."); }
	} else if(strcmp(equipo, "rojo", true) == 0) {
	    if(RedTeamPlayers < RedTeamMaxPlayers && target != INVALID_PLAYER_ID) {
			if(GetPVarInt(target, "GrupoPaintball") == 1)
				RedTeamPlayers -= 1;
			if(GetPVarInt(target, "GrupoPaintball") == 2)
				BlueTeamPlayers -= 1;
	        SetPVarInt(target, "GrupoPaintball", 1);
	        RedTeamPlayers += 1;
	        SendFMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Agregaste a %s al equipo rojo de Paintball.", GetPlayerNameEx(target));
			SendFMessage(target, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} El administrador %s te agreg� al equipo rojo de Paintball.", GetPlayerNameEx(playerid));
		} else { SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} �El equipo rojo est� lleno! Pod�s modificar el l�mite de jugadores con /pbmaxplayers."); }
	} else {
		SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} Los posibles equipos son rojo y azul.");
	}
	return 1;
}

CMD:pbreset(playerid, params[])
{
    PaintballPlay = 0,
    PaintballStart = 0,
    BlueTeamPlayers = 0,
	RedTeamPlayers = 0,
    BlueTeamMaxPlayers = 3,
    RedTeamMaxPlayers = 3,
	BlueTeamDeads = 0,
	RedTeamDeads = 0;
    SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} El paintball fue reiniciado.");
	foreach(new i : Player)	
	{
		if(GetPVarInt(i, "GrupoPaintball") != 0)
		{
	    	SetPVarInt(i, "GrupoPaintball", 0);
			SetHandItemAndParam(i, HAND_RIGHT, 0, 0);
			PhoneHand[i] = 0;
			SetPlayerSkin(i, PlayerInfo[i][pSkin]);
			if(GetPVarInt(i, "Descalificado") != 0)
			{
				SetPVarInt(i, "Descalificado", 0);
			}
		}
	}
    return 1;
}

CMD:pbayuda(playerid, params[])
{
	SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[LVL 20]{C8C8C8} /pbcrear - /pbmaxplayers - /pbinvitar - /pbcomenzar - /pbinfo");
	SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[LVL 20]{C8C8C8} /pbplayers - /pbreset - /pbskin - /gotopb - /pbreiniciar");
	SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Para iniciar un evento de Paintball deber�s, por empezar, crearlo con el comando /pbcrear");
	SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Una vez creado el evento, configura la cantidad m�xima de jugadores por equipo e invita a los usuarios.");
	SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Para comenzar con el evento, usa el comando /pbcomenzar. El evento permanecer� activo hasta que lo termines con /pbreset.");
	SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Para informaci�n detallada sobre el evento creado usa los comandos /pbinfo y /pbplayers.");
	SendClientMessage(playerid, COLOR_WHITE, "{878EE7}[INFO]{C8C8C8} Ante cualquier problema con el sistema, reinicia el paintball con /pbreset y vuelve a crearlo.");
	return 1;
}

CMD:pbinfo(playerid, params[])
{
	
	if(PaintballPlay == 0)
	    return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} No hay un paintball creado.");

	SendClientMessage(playerid, COLOR_WHITE, "{878EE7}---{C8C8C8} Informaci�n del Paintball {878EE7}---");
	SendFMessage(playerid, COLOR_WHITE, "{878EE7}Jugadores en el equipo {0004FF}azul{878EE7}: {C8C8C8} %d{878EE7}/{C8C8C8}%d", BlueTeamPlayers, BlueTeamMaxPlayers);
	SendFMessage(playerid, COLOR_WHITE, "{878EE7}Jugadores en el equipo {FF0000}rojo{878EE7}: {C8C8C8} %d{878EE7}/{C8C8C8}%d", RedTeamPlayers, RedTeamMaxPlayers);
	if(PaintballStart == 0) {
		SendClientMessage(playerid, COLOR_WHITE, "{878EE7}Evento iniciado: {C8C8C8} No.");
	} else {
		SendClientMessage(playerid, COLOR_WHITE, "{878EE7}Evento iniciado: {C8C8C8} S�.");
	}
	SendFMessage(playerid, COLOR_WHITE, "{878EE7}Jugadores descalificados por el equipo {0004FF}azul{878EE7}: {C8C8C8} %d", RedTeamDeads);
	SendFMessage(playerid, COLOR_WHITE, "{878EE7}Jugadores descalificados por el equipo {FF0000}rojo{878EE7}: {C8C8C8} %d", BlueTeamDeads);
	SendFMessage(playerid, COLOR_WHITE, "{878EE7}Skin del equipo {0004FF}azul{878EE7}: {C8C8C8} %d", RedTeamSkin);
	SendFMessage(playerid, COLOR_WHITE, "{878EE7}Skin del equipo {FF0000}rojo{878EE7}: {C8C8C8} %d", BlueTeamSkin);
	return 1;
}

CMD:pbplayers(playerid, params[])
{
	new string[128], count = 0, count2 = 0;

	if(PaintballPlay == 0)
		return SendClientMessage(playerid, COLOR_RED, "[ERROR]{C8C8C8} No hay un paintball creado.");

	SendClientMessage(playerid, COLOR_LIGHTGREEN, "{878EE7}[Paintball] {C8C8C8}Lista de jugadores equipo {FF0000}rojo{C8C8C8}:");
	foreach(new i : Player)
	{
	    if(GetPVarInt(i, "GrupoPaintball") == 1)
		{
			format(string, sizeof(string), "%s", GetPlayerNameEx(i));
			SendClientMessage(playerid, COLOR_WHITE, string);
			count++;
		}
	}
	if(count == 0)
	{
	    SendClientMessage(playerid, COLOR_WHITE, "No hay ning�n jugador en el equipo rojo.");
	}
	
	SendClientMessage(playerid, COLOR_LIGHTGREEN, "{878EE7}[Paintball] {C8C8C8}Lista de jugadores equipo {0004FF}azul{C8C8C8}:");
	foreach(new i : Player)
	{
	    if(GetPVarInt(i, "GrupoPaintball") == 2)
		{
			format(string, sizeof(string), "%s", GetPlayerNameEx(i));
			SendClientMessage(playerid, COLOR_WHITE, string);
			count2++;
		}
	}
	if(count2 == 0)
	{
	    SendClientMessage(playerid, COLOR_WHITE, "No hay ning�n jugador en el equipo azul.");
	}
	return 1;
}

CMD:gotopb(playerid, params[])
{
	if(GetPlayerState(playerid) == 2) {
		SetVehiclePos(GetPlayerVehicleID(playerid), POS_PAINTBALL_X, POS_PAINTBALL_Y, POS_PAINTBALL_Z);
	}else {
		SetPlayerPos(playerid, POS_PAINTBALL_X, POS_PAINTBALL_Y, POS_PAINTBALL_Z);
	}
	SetPlayerInterior(playerid, 0);
	SetPlayerVirtualWorld(playerid, 0);
	return 1;
}
	
CMD:pbreiniciar(playerid, params[])
{
	foreach(new i : Player) 
	{
		if(GetPVarInt(i, "GrupoPaintball") > 0)
		{
			SendClientMessage(i, COLOR_RED, "{878EE7}[INFO]{C8C8C8} El paintball se ha reiniciado ya que uno de los dos equipos gan�, vuelve a tu base para volver a jugar.");
			TogglePlayerControllable(i, true);
		}
	}
}
	
	
	
	
	
	
	
	

