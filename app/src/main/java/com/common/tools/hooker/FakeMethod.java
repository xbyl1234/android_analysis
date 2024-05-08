package com.common.tools.hooker;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
public @interface FakeMethod {
    int minSdk() default 0;

    int maxSdk() default 999;

    String methodName() default "";

    boolean needXposedParams() default false;

    boolean mustHook() default false;

    boolean staticHook() default false;

    String switchName() default "";
}