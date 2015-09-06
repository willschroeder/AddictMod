#include "quick_commands.h"
#include "eiface.h"
#include "edict.h"
#include "igameevents.h"
#include "CServerMan.h"
#include "CCommandMan.h"
#include "CModeMan.h"
#include "dp_string_tools.h"
#include "addict_globals.h"
#include "icvar.h"
#include "CTimer.h"
#include <string>

using namespace std;

extern CServerMan * pServerMan;
extern CCommandMan * pCommandMan;
extern CModeMan * pModeMan;
extern IVEngineServer * engine;
extern userOptions * pCurrentSettings;

const int MAX_SAY_LEN = 127;
bool quickCommands::announceGlobal = false;

void quickCommands::commandInterceptor(IGameEvent * anEvent)
{
	if(Q_strcmp(anEvent->GetName(),"player_say")==0)
	{
		const int commandUser = pServerMan->indexToSlot(anEvent->GetInt("userid"));
		const char * rawString = anEvent->GetString("text");
		int rawStringLen = Q_strlen(rawString);

		//dont let console call quick commands
		if(commandUser == -1)
			return;

		//voting
		announceGlobal = true;
		if(Q_strncmp(rawString,"votemap ",8)==0)
		{
			dp_string temp(2);
			temp.addString("map");
			temp.addString(rawString+8); 
			pModeMan->userCommandExecute("ad_vote",temp.getMaxItems(),temp.dpList,commandUser);
			announceGlobal = false;
			return;
		}

		else if(Q_strncmp(rawString,"voteslay ",9)==0)
		{
			dp_string temp(2);
			temp.addString("slay");
			temp.addString(rawString+9);
			pModeMan->userCommandExecute("ad_vote",temp.getMaxItems(),temp.dpList,commandUser);
			announceGlobal = false;
			return;
		}

		else if(Q_strncmp(rawString,"votekick ",9)==0 || Q_strncmp(rawString,"voteban ",8)==0)
		{
			dp_string temp(2);
			temp.addString("kick");
			temp.addString(rawString+9);
			pModeMan->userCommandExecute("ad_vote",temp.getMaxItems(),temp.dpList,commandUser);
			announceGlobal = false;
			return;
		}
		announceGlobal = false;

		//music
		//Name is in this format 1/0visible|name, so only the first 2 chars are visible or not
		if(rawString[0]=='!' && pServerMan->PlayerList[commandUser]->adminLevel > PLAYER_NORMAL) //Block music commands from pubbies
		{
			if(Q_strcmp(rawString+1,"stop")==0)
			{
				dp_string temp(2);
				temp.addString(pServerMan->PlayerList[commandUser]->playerSteamID);
				temp.addString("#blank");
				pCommandMan->commandExecute("ad_browser",temp.dpList,temp.getMaxItems());
				return;
			}
			else if(Q_strcmp(rawString+1,"music")==0)
			{
				//List Music
				dp_string temp(4);
				temp.addString(pServerMan->PlayerList[commandUser]->playerSteamID);
				temp.addString(" "); //no file name
				temp.addString("#url");
				temp.addString("true");
				pCommandMan->commandExecute("ad_browser",temp.dpList,temp.getMaxItems());
				return;
			}
			else if(Q_strncmp(rawString+1,"song",4)==0)
			{
				if(rawStringLen <= 6)
				{
					dp_string temp(2);
					temp.addString("#all");
					temp.addString("#blank"); 
					pCommandMan->commandExecute("ad_browser",temp.dpList,temp.getMaxItems(),commandUser);
					return;
				}
				else
				{
					dp_string temp(3);
					temp.addString("#all");
					temp.addString(rawString+6); 
					temp.addString("#url");
					pCommandMan->commandExecute("ad_browser",temp.dpList,temp.getMaxItems(),commandUser);
					return;
				}

			}
			else if(Q_strncmp(rawString+1,"play",4)==0)
			{
				if(rawStringLen <= 6)
				{
					dp_string temp(2);
					temp.addString(pServerMan->PlayerList[commandUser]->playerSteamID);
					temp.addString("#blank"); 
					pCommandMan->commandExecute("ad_browser",temp.dpList,temp.getMaxItems());
					return;
				}
				else
				{
					dp_string temp(3);
					temp.addString(pServerMan->PlayerList[commandUser]->playerSteamID);
					temp.addString(rawString+6); 
					temp.addString("#url");
					pCommandMan->commandExecute("ad_browser",temp.dpList,temp.getMaxItems());
					return;
				}

			}
			else
			{
				//URL Line parser
				bool beVisible = false;
	
				//Search names
				int quickMatch = -1;
				for(int i = 0; i < pCurrentSettings->numQuickUrl;i++)
					if(Q_strcmp(rawString+1,pCurrentSettings->quickUrlName->dpList[i])==0)
					{
						quickMatch = i;
						break;
					}

				if(quickMatch != -1)
				{
					if(Q_strncmp("v|",pCurrentSettings->quickUrl->dpList[quickMatch],2)==0)
					{
						dp_string temp(3);
						temp.addString(pServerMan->PlayerList[commandUser]->playerSteamID);
						temp.addString(pCurrentSettings->quickUrl->dpList[quickMatch]+2);
						temp.addString("true");
						pCommandMan->commandExecute("ad_browser",temp.dpList,temp.getMaxItems());
					}
					else
					{
						dp_string temp(2);
						temp.addString(pServerMan->PlayerList[commandUser]->playerSteamID);
						temp.addString(pCurrentSettings->quickUrl->dpList[quickMatch]);
						pCommandMan->commandExecute("ad_browser",temp.dpList,temp.getMaxItems());
					}

				}

				return;

			}
		}

		//list
		if(Q_strncmp("list ",rawString,5)==0)
		{
			//registered
			if(Q_strcmp(rawString+5,"registered")==0)
			{
				string temp= "say The registered users are: ";
				bool firstListed = false;
				for(int i = 0; i < pServerMan->getMaxPlayers();i++)
					if(pServerMan->PlayerList[i] != NULL)
						if(pServerMan->PlayerList[i]->playerAdminName != NULL)
						{
							//Formatting purposes
							if(firstListed) temp.append(", "); else firstListed = true; 

							//Check to see name len breach, print current, then continue adding
							int newNameLen = Q_strlen(pServerMan->PlayerList[i]->playerAdminName);
							if(temp.length() + newNameLen > MAX_SAY_LEN)
							{
								temp.append("\n");
								engine->ServerCommand(temp.c_str());
								temp = "say ";
							}
							temp.append(pServerMan->PlayerList[i]->playerAdminName);
						}
						temp.append("\n");
						engine->ServerCommand(temp.c_str());
						return;
			}

			//protected
			if(Q_strcmp(rawString+5,"protected")==0)
			{
				string temp= "say The protected users are: ";
				bool firstListed = false;
				for(int i = 0; i < pServerMan->getMaxPlayers();i++)
					if(pServerMan->PlayerList[i] != NULL)
						if(pServerMan->PlayerList[i]->adminLevel >= PLAYER_PROTECTED)
						{
							//Formatting purposes
							if(firstListed) temp.append(", "); else firstListed = true; 

							//Check to see name len breach, print current, then continue adding
							int newNameLen = Q_strlen(pServerMan->PlayerList[i]->playerName);
							if(temp.length() + newNameLen > MAX_SAY_LEN)
							{
								temp.append("\n");
								engine->ServerCommand(temp.c_str());
								temp = "say ";
							}
							temp.append(pServerMan->PlayerList[i]->playerName);
						}
						temp.append("\n");
						engine->ServerCommand(temp.c_str());
						return;
			}

			//pubbie
			if(Q_strcmp(rawString+5,"pubbies")==0)
			{
				string temp= "say The pubbies are: ";
				bool firstListed = false;
				for(int i = 0; i < pServerMan->getMaxPlayers();i++)
					if(pServerMan->PlayerList[i] != NULL)
						if(pServerMan->PlayerList[i]->adminLevel == PLAYER_NORMAL)
						{
							{
								//Formatting purposes
								if(firstListed) temp.append(", "); else firstListed = true; 

								//Check to see name len breach, print current, then continue adding
								int newNameLen = Q_strlen(pServerMan->PlayerList[i]->playerName);
								if(temp.length() + newNameLen > MAX_SAY_LEN)
								{
									temp.append("\n");
									engine->ServerCommand(temp.c_str());
									temp = "say ";
								}
								temp.append(pServerMan->PlayerList[i]->playerName);
							}
						}
						temp.append("\n");
						engine->ServerCommand(temp.c_str());
						return;
			}

			//admins
			if(Q_strcmp(rawString+5,"admins")==0)
			{
				string temp= "say The admins are: ";
				bool firstListed = false;
				for(int i = 0; i < pServerMan->getMaxPlayers();i++)
					if(pServerMan->PlayerList[i] != NULL)
						if(pServerMan->PlayerList[i]->adminLevel >= PLAYER_MANAGEMENT)
						{
							{
								//Formatting purposes
								if(firstListed) temp.append(", "); else firstListed = true; 

								//Check to see name len breach, print current, then continue adding
								int newNameLen = Q_strlen(pServerMan->PlayerList[i]->playerName);
								if(temp.length() + newNameLen > MAX_SAY_LEN)
								{
									temp.append("\n");
									engine->ServerCommand(temp.c_str());
									temp = "say ";
								}
								temp.append(pServerMan->PlayerList[i]->playerName);
							}
						}
						temp.append("\n");
						Warning("%s\n",temp.c_str());
						engine->ServerCommand(temp.c_str());
						return;
			}
		}


		//Whatis
		if(Q_strncmp(rawString,"whatis ",7)==0)
		{
			if(Q_strlen(rawString+7) > 1 && rawString[7] != '#')
			{
				int * selectedPlayers = new int[pServerMan->getMaxPlayers()];
				int selectedPlayersCount;
				
				selectedPlayersCount = pServerMan->strToSlot(rawString+7,selectedPlayers);
				if(selectedPlayersCount < 1)
				{
					engine->ServerCommand("say No one was found matching the given string.\n");
					delete selectedPlayers;
					return;
				}

				char buffer[100];
				
				for(int i = 0; i < selectedPlayersCount; i++)
					switch(pServerMan->PlayerList[selectedPlayers[i]]->adminLevel)
					{
					case PLAYER_NORMAL:
						Q_snprintf(buffer,100,"say Player %s is a pubbie.\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName);
						engine->ServerCommand(buffer);
						break;

					case PLAYER_PROTECTED:
						Q_snprintf(buffer,100,"say Player %s is protected.\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName);
						engine->ServerCommand(buffer);
						break;

					case PLAYER_MANAGEMENT:
						Q_snprintf(buffer,100,"say Player %s is a management admin.\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName);
						engine->ServerCommand(buffer);
						break;

					case PLAYER_TOTAL:
						Q_snprintf(buffer,100,"say Player %s a full powered admin.\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName);
						engine->ServerCommand(buffer);
						break;

					case DEV_ONLY:
						Q_snprintf(buffer,100,"say Player %s is Addict, the creator/developer of this magical plugin.\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName);
						engine->ServerCommand(buffer);
						break;
					}

				delete selectedPlayers;
			}
			else
				engine->ServerCommand("say Please be more specific.\n");
			return;
		}

		//Whois
		if(Q_strncmp(rawString,"whois ",6)==0)
		{
			if(Q_strlen(rawString+6) > 1 && rawString[6] != '#')
			{
				int * selectedPlayers = new int[pServerMan->getMaxPlayers()];
				int selectedPlayersCount;

				selectedPlayersCount = pServerMan->strToSlot(rawString+6,selectedPlayers);
				if(selectedPlayersCount < 1)
				{
					engine->ServerCommand("say No one was found matching the given string.\n");
					delete selectedPlayers;
					return;
				}

				char buffer[100];
				for(int i = 0; i < selectedPlayersCount; i++)
				{
					//Admin name unknown
					if(pServerMan->PlayerList[selectedPlayers[i]]->playerAdminName == NULL)
					{
						Q_snprintf(buffer,100,"say The player %s is not registered with the server, so they are a pubbie.\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName);
						engine->ServerCommand(buffer);

					}
					else
					{
						Q_snprintf(buffer,100,"say Player %s is registered as %s with the server.\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName,pServerMan->PlayerList[selectedPlayers[i]]->playerAdminName);
						engine->ServerCommand(buffer);
					}
				}			

				delete selectedPlayers;
			}
			else
				engine->ServerCommand("say Please be more specific.\n");
			return;
		}

		
		//Announce nextmap
		if(Q_strncmp(rawString,"nextmap",7)==0)
		{
			dp_string temp(1);
			temp.addString("");
			pCommandMan->commandExecute("nextmap",temp.dpList,temp.getMaxItems());
		}

		//Timeleft using ICvar mp_timelimit
		if(Q_strncmp(rawString,"timeleft",8)==0)
		{
			ConVar * pConVar = cvar->FindVar( "mp_timelimit" ); 
			float timeLimit = pConVar->GetFloat();
			
			if(timeLimit == 0)
			{
				engine->ServerCommand("say Time Unlimited\n");
				return;
			}

			float timeLeft = ((timeLimit * 60) - CTimerMan::getCurrentTime()); //in seconds
			char buffer[100];
			Q_snprintf(buffer,100,"say %i minutes OR %f seconds left.\n",int((timeLeft/60)),timeLeft);
			engine->ServerCommand(buffer);
			return;

		}

		//FF on/off using Ivar mp_friendlyfire
		if(Q_strncmp(rawString,"ff",2)==0)
		{
			ConVar * pConVar = cvar->FindVar( "mp_friendlyfire" ); 
			
			if(pConVar->GetBool())
				engine->ServerCommand("say Friendly Fire is ON!\n");
			else
				engine->ServerCommand("say Friendly Fire is OFF!\n");

			return;
		}

		/* NOTE: im not using this way because i dont know if i can blank the say text, so the command would be echoed into console
		//Admin commands starting with @, dont bother checking for levels, just pass the command user with the command exec function
		if(rawString[0] == '@')
		{
			//Admin Say
			if(Q_strncmp(rawString+1,"say ",4)==0)
			{
				
			}
		}
		*/
	}

}
