#pragma once
#include "../GameScene.h"

struct Tile
{
	enum TileState
	{
		closed
		,open
		,numStates
	};
	Tile() { state = TileState::closed; };
	~Tile() { delete m_pButton; }
	TileState state;
	int m_bombsAround;
	Button* m_pButton;

	void Draw() { if (m_pButton) m_pButton->Draw(); };

};

class RoomScene : public GameScene
{
	int m_buttonWidth, m_buttonHeight, m_numTilesX, m_numTilesY;
	static Button* m_pQuitGameButton;
	static Button* m_pBackgroundButton;

	bool m_rendering = false;
	bool m_editingScene = false;
	
	std::vector<TextField*> m_pRoomPlayerList;
	std::vector<Tile*> m_pTileMap;

	static void OnTilePressed(Button* pButton, int tileIndex);

public:
	RoomScene();
	~RoomScene();

	virtual void UpdateRender() override;

	virtual void Update(Proto::ServerResponse* pResponse) override;

	virtual void OnEvent(const Event* pEvent);
	static void OnQuitButton(Button* pButton, int);


private:

};