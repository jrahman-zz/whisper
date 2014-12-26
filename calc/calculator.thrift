namespace cpp example

service Calculator shared.SharedService {
    
    void ping();

    i64 add(1: i32 num1, 2: i32 num2),
    
    i32 calculate(1: i32 logid, 2: Work w) throws (1: InvalidOperation ouch),

    oneway void zip()
}


