import ClayJava.Clay_RenderCommandArray;

public class App {
  static {
    System.loadLibrary("App");
  }
  public native void hello(String name);
  
  public native void Clay_Create();
  
  public native void Clay_Destroy();
  
  /**
   * Sets the layout dimensions for Clay. Call this when the window size changes.
   * @param screenWidth The current width of the screen.
   * @param screenHeight The current height of the screen.
   */
  public native void setLayoutDimensions(int screenWidth, int screenHeight);
  
  /**
   * Begins the Clay layout process. Must be called before declaring layout elements.
   */
  public native void beginLayout();
  
  public native Clay_RenderCommandArray endLayout();
  
  public static void run() {
    App app = new App();
    //app.hello("World");
    app.setLayoutDimensions(800, 600);
    app.Clay_Create();
    app.beginLayout();
    Clay_RenderCommandArray renderCommands = app.endLayout();
    System.out.println("Render commands: " + renderCommands.length);
    app.Clay_Destroy();
  }
}
