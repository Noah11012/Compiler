typedef struct
{
    char buffer[512]; // Will this be enough? For now yes.
    int length;
} StringBuilder;

StringBuilder CreateStringBuilder(void)
{
    StringBuilder builder;
    builder.length = 0;

    return builder;
}

void PushToStringBuilder(StringBuilder *builder, char const *format, ...)
{
    va_list list;
    va_start(list, format);

    int chars_written = vsnprintf(builder->buffer + builder->length, 512, format, list);

    builder->length += chars_written;
    Assert(builder->length < 512);

    va_end(list);
}

char *FinalizeStringBuilder(StringBuilder *builder)
{
    char *result = malloc(builder->length + 1);
    Assert(result);

    memcpy(result, builder->buffer, builder->length);
    result[builder->length] = 0;

    return result;
}

