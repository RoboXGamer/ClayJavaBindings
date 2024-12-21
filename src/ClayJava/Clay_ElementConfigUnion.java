package ClayJava;

public class Clay_ElementConfigUnion {
  public Clay_ElementConfigType type;
  public Object config; // Holds the actual configuration object
  
  public <T> T getConfig(Class<T> clazz) {
    if (clazz.isInstance(config)) {
      return clazz.cast(config);
    }
    return null; // Or throw an exception
  }
  
  public Clay_ElementConfigUnion(Clay_ElementConfigType type, Object config) {
    this.type = type;
    this.config = config;
  }
}