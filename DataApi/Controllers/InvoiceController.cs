using Microsoft.AspNetCore.Mvc;

namespace DataApi.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class InvoiceController : ControllerBase
    {
        private readonly ILogger<InvoiceController> _logger;
        private IList<Invoice> _invoices;

        public InvoiceController(ILogger<InvoiceController> logger)
        {
            _logger = logger;
            _invoices = new List<Invoice>();
            GenerateInvoices();
        }

        private void GenerateInvoices()
        {
            var today = new DateOnly(2024, 1, 1);
            var currentInvoiceDay = new DateOnly(today.Year, today.Month, 1);
            Random random = new Random();

            while (currentInvoiceDay.Year == today.Year)
            {
                for (int index = 0; index <= random.Next(100, 1000); index++)
                {
                    _invoices.Add(new Invoice(
                        currentInvoiceDay, 
                        Enumerable.Range(1, random.Next(1, 10)).Select(test => new InvoiceLine(random.Next(10, 1000))).ToList()
                    ));
                }

                currentInvoiceDay = currentInvoiceDay.AddDays(1);
            }
        }

        [HttpGet]
        public InvoicesViewModel Get()
        {
            return CreateInvoiceViewModel(_invoices);
        }

        [HttpGet]
        [Route("{date}")]
        public InvoicesViewModel Get(DateOnly date)
        {
            Console.WriteLine(date.ToString());
            return CreateInvoiceViewModel(_invoices.Where(invoice => invoice.Date.Equals(date)));
        }

        private InvoicesViewModel CreateInvoiceViewModel(IEnumerable<Invoice> invoices)
        {
            Console.WriteLine(invoices.Count());
            return new(invoices.First().Date, invoices.Last().Date, invoices.Count(), invoices);
        }
    }
}
