#include <SFML/Graphics.hpp>

// Always include the necessary header files.
// These mostly have the form SFGUI/<Widget name>.hpp
#include <SFGUI/Window.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Box.hpp>

int main() {
	// Create the main SFML window
	sf::RenderWindow app_window( sf::VideoMode( 800, 600 ), "SFGUI Box Example" );

	// Create our main SFGUI window
	sfg::Window::Ptr window;
	window = sfg::Window::Create();
	window->SetTitle( "Title" );

	// Since only being able to add one widget to a window is very limiting
	// there are Box widgets. They are a subclass of the Container class and
	// can contain an unlimited amount of child widgets. Not only that, they
	// also have the ability to lay out your widgets nicely.

	// Create our box smart pointer.
	sfg::Box::Ptr box;

	// Create the box itself.
	// For layout purposes we must specify in what direction new widgets
	// should be added, horizontally or vertically.
	box = sfg::Box::Create( sfg::Box::HORIZONTAL );

	sfg::Button::Ptr button1 = sfg::Button::Create();
	sfg::Button::Ptr button2 = sfg::Button::Create();
	sfg::Label::Ptr label = sfg::Label::Create();

	button1->SetLabel( "Foo" );
	button2->SetLabel( "Bar" );
	label->SetText( "Baz" );

	// To add our widgets to the box we use the Pack() method instead of the
	// Add() method. This makes sure the widgets are added and layed out
	// properly in the box.
	box->Pack( button1 );
	box->Pack( label );
	box->Pack( button2 );

	// Just as with the window we can set the spacing between widgets
	// withing a box.
	box->SetSpacing( 5.f );

	// Finally we add our box to the window as it's only child.
	// Notice that we don't have to add the children of a box to it's parent
	// Because all children and grandchildren and .... are automatically
	// considered descendents of the parent.
	window->Add( box );

	// Start the game loop
	while ( app_window.IsOpened() ) {
		// Process events
		sf::Event event;

		while ( app_window.PollEvent( event ) ) {
			// Handle events
			window->HandleEvent( event );

			// Close window : exit
			if ( event.Type == sf::Event::Closed ) {
				app_window.Close();
			}
		}

		// Clear screen
		app_window.Clear();

		// Draw the window
		window->Expose( app_window );

		// Update the window
		app_window.Display();
	}

	return EXIT_SUCCESS;
}
