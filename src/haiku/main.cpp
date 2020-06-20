#include <cstdio>
#include <Be.h>

struct MyBox : public BBox {
	MyBox(BRect r) : BBox(r, "Activity", B_FOLLOW_ALL){}
	void KeyDown(const char* bytes, int32 numBytes);
};

struct MainWindow : public BWindow {
	MainWindow();
	void FrameResized(float width, float height);
	bool QuitRequested();
private:
	char current_word[7];
	float button_height;
	MyBox* activity;
	BButton* newgame;
	BButton* shuffle;
};


MainWindow::MainWindow() : BWindow(BRect(100, 100, 400, 400), "Mingling Masjuclecc", B_TITLED_WINDOW, 0)
{
	BRect r = this->Bounds();
	newgame = new BButton(BRect(0, 0, (r.right - r.left) / 2, 0), "newgame", "New Game", NULL, 0);
	shuffle = new BButton(BRect(0, 0, (r.right - r.left) / 2, 0), "shuffle", "Shuffle", NULL, 0);

	BRect s = newgame->Bounds();
	button_height = s.bottom;
	
	activity = new MyBox(BRect(0, 0, r.right - r.left, r.bottom - r.top - button_height));
	newgame->MoveTo(0, r.bottom - r.top - button_height);
	shuffle->MoveTo((r.right - r.left) / 2, r.bottom - r.top - button_height);
	
	this->AddChild(newgame);
	this->AddChild(shuffle);
	this->AddChild(activity);
	activity->MakeFocus();
	
	memset(current_word, 0, 7);
}

void MainWindow::FrameResized(float width, float height)
{
	newgame->MoveTo(0, height - button_height);
	newgame->ResizeTo(width / 2, button_height);

	shuffle->MoveTo(width / 2, height - button_height);
	shuffle->ResizeTo(width/2, button_height);
}

bool MainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void MyBox::KeyDown(const char* bytes, int32 numBytes)
{
	printf("Key down: %c\n", *bytes);
}


int main(int argc, char **argv)
{
	BApplication* app = new BApplication("application/game");
	MainWindow* win = new MainWindow();
	
	win->Show();
	app->Run();
}
