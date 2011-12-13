#include <SFGUI/Window.hpp>
#include <SFGUI/Engine.hpp>
#include <SFGUI/Context.hpp>

namespace sfg {

Window::Window() :
	Bin(),
	m_style( Toplevel ),
	m_dragging( false ),
	m_resizing( false )
{
}

Window::~Window() {
}

Window::Ptr Window::Create() {
	Window::Ptr window( new Window );

	window->RequestResize();

	return window;
}

RenderQueue* Window::InvalidateImpl() const {
	return Context::Get().GetEngine().CreateWindowDrawable( DynamicPointerCast<const Window>( shared_from_this() ) );
}

void Window::SetTitle( const sf::String& title ) {
	m_title = title;
	Invalidate();
}

const sf::String& Window::GetTitle() const {
	return m_title;
}

sf::FloatRect Window::GetClientRect() const {
	sf::FloatRect clientrect( 0, 0, GetAllocation().Width, GetAllocation().Height );
	float border_width( Context::Get().GetEngine().GetProperty<float>( "BorderWidth", shared_from_this() ) );
	float gap( Context::Get().GetEngine().GetProperty<float>( "Gap", shared_from_this() ) );

	clientrect.Left += border_width + gap;
	clientrect.Top += border_width + gap;
	clientrect.Width -= 2 * border_width + 2 * gap;
	clientrect.Height -= 2 * border_width + 2 * gap;

	if( HasStyle( Titlebar ) ) {
		float title_height( Context::Get().GetEngine().GetProperty<float>( "TitleHeight", shared_from_this() ) );
		clientrect.Top += title_height;
		clientrect.Height -= title_height;
	}

	return clientrect;
}

void Window::HandleAllocationChange( const sf::FloatRect& /*old_allocation*/ ) {
	if( !GetChild() ) {
		return;
	}

	GetChild()->SetAllocation( GetClientRect() );
}

void Window::SetStyle( int style ) {
	m_style = style;

	// Make sure dragging and resizing operations are cancelled.
	m_dragging = false;
	m_resizing = false;

	RequestResize();
	Invalidate();

	if( GetChild() ) {
		GetChild()->SetAllocation( GetClientRect() );
	}
}

int Window::GetStyle() const {
	return m_style;
}

bool Window::HasStyle( Style style ) const {
	return (m_style & style) == style;
}

sf::Vector2f Window::CalculateRequisition() {
	float visual_border_width( Context::Get().GetEngine().GetProperty<float>( "BorderWidth", shared_from_this() ) );
	float gap( Context::Get().GetEngine().GetProperty<float>( "Gap", shared_from_this() ) );
	sf::Vector2f requisition( 2 * visual_border_width + 2 * gap, 2 * visual_border_width + 2 * gap );

	if( HasStyle( Titlebar ) ) {
		requisition.y += Context::Get().GetEngine().GetProperty<float>( "TitleHeight", shared_from_this() );
	}

	if( GetChild() ) {
		requisition += GetChild()->GetRequisition();
	}
	else {
		requisition.x = std::max( 50.f, requisition.x );
		requisition.y = std::max( 50.f, requisition.y * 2.f );
	}

	return requisition;
}

const std::string& Window::GetName() const {
	static const std::string name( "Window" );
	return name;
}

void Window::HandleMouseButtonEvent( sf::Mouse::Button button, bool press, int x, int y ) {
	if( button != sf::Mouse::Left ) {
		return;
	}

	if( !press ) {
		m_dragging = false;
		m_resizing = false;
		return;
	}

	// Check for mouse being inside the title area.
	sf::FloatRect area(
		GetAllocation().Left,
		GetAllocation().Top,
		GetAllocation().Width,
		Context::Get().GetEngine().GetProperty<float>( "TitleHeight", shared_from_this() )
	);

	if( area.Contains( static_cast<float>( x ), static_cast<float>( y ) ) ) {
		if( HasStyle( Titlebar ) && !m_dragging ) {
			m_dragging = true;
			m_resizing = false;

			m_drag_offset = sf::Vector2f(
				static_cast<float>( x ) - GetAllocation().Left,
				static_cast<float>( y ) - GetAllocation().Top
			);
		}
	}
	else {
		float handle_size( Context::Get().GetEngine().GetProperty<float>( "HandleSize", shared_from_this() ) );

		area.Left = GetAllocation().Left + GetAllocation().Width - handle_size;
		area.Top = GetAllocation().Top + GetAllocation().Height - handle_size;
		area.Width = handle_size;
		area.Height = handle_size;

		if( area.Contains( static_cast<float>( x ), static_cast<float>( y ) ) ) {
			m_dragging = false;
			m_resizing = true;

			m_drag_offset = sf::Vector2f(
				handle_size - static_cast<float>( x ) + GetAllocation().Left + GetAllocation().Width - handle_size,
				handle_size - static_cast<float>( y ) + GetAllocation().Top + GetAllocation().Height - handle_size
			);
		}
	}

}

void Window::HandleMouseMoveEvent( int x, int y ) {
	if( m_dragging ) {
		SetPosition(
			sf::Vector2f(
				static_cast<float>( x ) - m_drag_offset.x,
				static_cast<float>( y ) - m_drag_offset.y
			)
		);
	}
	else if( m_resizing ) {
		SetAllocation(
			sf::FloatRect(
				GetAllocation().Left,
				GetAllocation().Top,
				std::max( GetRequisition().x, static_cast<float>( x ) + m_drag_offset.x - GetAllocation().Left ),
				std::max( GetRequisition().y, static_cast<float>( y ) + m_drag_offset.y - GetAllocation().Top )
			)
		);
	}
}

void Window::HandleAdd( const Widget::Ptr& child ) {
	Bin::HandleAdd( child );

	if( GetChild() ) {
		// Reset allocation so the window will be as large as required.
		SetAllocation( sf::FloatRect( GetAllocation().Left, GetAllocation().Top, 1.f, 1.f ) );
		RequestResize();
	}
}

}
