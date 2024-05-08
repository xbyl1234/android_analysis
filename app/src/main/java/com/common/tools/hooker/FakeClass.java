package com.common.tools.hooker;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
public @interface FakeClass {
    String ClassName() default "";
    String ClassNameAndroid9() default "";
    String ClassNameAndroid10() default "";
}
