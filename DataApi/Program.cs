using DataApi;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddSingleton<DataSingleton>();
builder.Services.AddControllers();

var app = builder.Build();

app.UseHttpsRedirection();

app.UseAuthorization();

app.MapControllers();

app.Run();
